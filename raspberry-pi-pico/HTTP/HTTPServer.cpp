#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>

#include <lumos-arduino/Logger.h>

#include "ConnectionContext.h"
#include "HTTPRequest.h"
#include "HTTPRequestParser.h"
#include "HTTPServer.h"

bool HTTPServer::init(uint16_t const port) {
  if (port == 0 || port > 0xFFFF) {
    logger << "Invalid port: " << port << std::endl;
    return false;
  }

  auto pcb = tcp_new();
  if (pcb == nullptr) {
    logger << "Error calling tcp_new" << std::endl;
    return false;
  }

  if (err_t const err = tcp_bind(pcb, IP_ADDR_ANY, port); err != ERR_OK) {
    logger << "Error calling tcp_bind (" << port << "): " << errToString(err) << std::endl;
    tcp_abort(pcb);
    return false;
  }

  auto listenPcb = tcp_listen(pcb);
  if (listenPcb == nullptr) {
    logger << "Error calling tcp_listen" << std::endl;
    tcp_abort(pcb);
    return false;
  }

  tcp_arg(listenPcb, this);
  tcp_accept(listenPcb, onAccept);
  logger << "HTTP server listening on port " << port << std::endl;

  return true;
}

err_t HTTPServer::onAccept(void *arg, tcp_pcb *newpcb, [[maybe_unused]] err_t err) {
  auto *server = static_cast<HTTPServer*>(arg);

  // Associate a new context with the pcb
  auto *context = new ConnectionContext{server, newpcb};
  tcp_arg(newpcb, context);

  if (HTTP_DEBUG) {
    std::string const remoteIpStr = ipaddr_ntoa(&newpcb->remote_ip);
    logger << *context << "Accepted new connection from " << remoteIpStr << ":" << newpcb->remote_port << std::endl;
  }

  // Register callbacks
  tcp_recv(newpcb, onReceive);
  tcp_sent(newpcb, onSent);
  tcp_err(newpcb, onError);

  return ERR_OK;
}

err_t HTTPServer::onReceive(void *arg, tcp_pcb *tpcb, pbuf *p, err_t const err) {
  auto const context = static_cast<ConnectionContext*>(arg);

  // If there was an error, then abort the connection
  if (err != ERR_OK) {
    logger << *context << "Error in onReceive: " <<  errToString(err) << std::endl;
    abortConnection(context);
    return ERR_ABRT;
  }

  // If pbuf is null then the client closed the connection. Close our end of this connection.
  if (p == nullptr) {
    if (HTTP_DEBUG) {
      logger << *context << "Connection closed by client" << std::endl;
    }
    closeConnection(context);
    return ERR_OK;
  }

  // Get the data payload from the supplied buffer
  context->inData.append(static_cast<char*>(p->payload), p->len);
  pbuf_free(p);

  // Parse the raw request payload
  context->parser.parse(context->inData);
  HTTPRequest const &request = context->parser.request();

  if (HTTP_DEBUG) {
    logger << *context << request;
  }

  // Was the request invalid?
  if (context->parser.state() == HTTPRequestParser::RequestState::FAILED) {
    logger << *context << "Invalid HTTP request" << std::endl;
    context->server->sendResponse(context, {400, "text/plain", "Invalid request"});
    closeConnection(context);
    return ERR_OK;
  }

  // Was the request complete?
  if (context->parser.state() == HTTPRequestParser::RequestState::COMPLETE) {
    // Match the method and path to a handler, otherwise return a not found error
    std::string const handlersKey = makeHandlersKey(request.method, request.path);
    auto it = context->server->handlers.find(handlersKey);
    if (it != context->server->handlers.end()) {
      auto const response = it->second(request);  // Call the handler
      context->server->sendResponse(context, response);
    } else {
      context->server->sendResponse(context, {404, "text/plain", "Not found"});
    }
  }

  return ERR_OK; // Return ERR_OK to continue receiving
}

err_t HTTPServer::onSent(void *arg, tcp_pcb *tpcb, u16_t const len) noexcept {
  auto const context = static_cast<ConnectionContext*>(arg);
  context->bytesSent += len;

  if (context->bytesSent >= context->outData.length()) {
    // The entire response has been written, nothing else to send
    context->reset();

    if (HTTP_DEBUG) {
      logger << *context << "HTTP Response complete, final " <<  len << " bytes were sent"<< std::endl;
    }
    return ERR_OK;
  }

  // Try to send the remaining bytes
  if (!context->remainingOutData.empty()) {
    if (HTTP_DEBUG) {
      auto const remainingBytes = context->remainingOutData.length();
      logger << *context << "Sent " << len << " bytes, " << "writing the remaining " << remainingBytes << " bytes" << std::endl;
    }
    writeResponseBytes(context);

    return ERR_OK;
  }

  // Nothing more to send, just acknowledge the bytes that were sent
  if (HTTP_DEBUG) {
    logger << *context << "Sent " << len << " bytes" << std::endl;
  }

  return ERR_OK;
}

void HTTPServer::onError(void *arg, err_t const err) noexcept {
  auto const context = static_cast<ConnectionContext*>(arg);

  if (!context) {
    logger << "Error: null connection context in onError. Error: " << errToString(err) << std::endl;
    return;
  }

  logger << *context << "Connection error: " << errToString(err) << std::endl;
  tcp_abort(context->pcb);
  delete context;
}

err_t HTTPServer::sendResponse(ConnectionContext *context, HTTPResponse const &response) {
  std::ostringstream responseStream;

  // Status line
  responseStream << "HTTP/1.1 " << response.status_code << " ";
  switch (response.status_code) {
    case 200: responseStream << "OK"; break;
    case 400: responseStream << "Bad Request"; break;
    case 404: responseStream << "Not Found"; break;
    case 500: responseStream << "Internal Server Error"; break;
    default: responseStream << "Unknown Status"; break;
  }
  responseStream << "\r\n";

  // Add headers
  responseStream << "Content-Type: " << response.contentType << "\r\n";
  responseStream << "Content-Length: " << response.body.size() << "\r\n";
  responseStream << "\r\n";

  // Add the headers and body to a buffer held in the connection context
  context->outData = responseStream.str();
  context->outData += response.body;

  // Make the remaining data a string view that we can keep shortening with little overhead
  context->remainingOutData = context->outData;

  return writeResponseBytes(context);
}

err_t HTTPServer::writeResponseBytes(ConnectionContext *context) {
  auto const remainingBytes = context->remainingOutData.length();

  if (remainingBytes == 0) {
    logger << *context << "No data remaining to write" << std::endl;
    return ERR_ARG;
  }

  // Ensure the response fits within the TCP buffer
  u16_t const bytesToWrite = std::min(remainingBytes, static_cast<size_t>(tcp_sndbuf(context->pcb)));
  if (bytesToWrite == 0) {
    logger << *context << "Write buffer full, unable to write any of the remaining " << remainingBytes << " bytes" << std::endl;
    return ERR_MEM;
  }

  if (HTTP_DEBUG) {
    logger << *context << "Writing " << bytesToWrite << " of " << remainingBytes << " bytes" << std::endl;
  }

  err_t err = tcp_write(context->pcb, context->remainingOutData.data(), bytesToWrite, TCP_WRITE_FLAG_COPY);
  if (err != ERR_OK) {
    logger << *context << "Error in tcp_write: " << errToString(err) << std::endl;
    abortConnection(context);
    return err;
  }

  // Flush the buffer explicitly
  err = tcp_output(context->pcb);
  if (err != ERR_OK) {
    logger << *context << "Error in tcp_output: " << errToString(err) << std::endl;
    abortConnection(context);
    return err;
  }

  // The next chunk of the remaining bytes were written
  context->remainingOutData = context->remainingOutData.substr(bytesToWrite);

  return ERR_OK;
}

void HTTPServer::closeConnection(ConnectionContext const *context) noexcept {
  if (!context) {
    logger << *context << "Attempted to close a null connection context" << std::endl;
    return;
  }

  if (err_t const err = tcp_close(context->pcb); err != ERR_OK) {
    logger << *context << "Error in tcp_close: " << errToString(err) << std::endl;
    tcp_abort(context->pcb);
  } else {
    if (HTTP_DEBUG) {
      logger << *context << "Connection closed" << std::endl;
    }
  }
  delete context;
}

void HTTPServer::abortConnection(ConnectionContext const *context) noexcept {
  if (!context) {
    logger << *context << "Attempted to abort a null connection context" << std::endl;
    return;
  }

  tcp_abort(context->pcb);
  logger << *context << "Connection aborted" << std::endl;
  delete context;
}

/**
 * Combines the HTTP method and path into a single key.
 * The format is "<METHOD> <PATH>", e.g., "GET /index.html".
 *
 * @param method The HTTP method (e.g., "GET", "POST").
 * @param path The request path (e.g., "/index.html").
 * @return A single string representing the combined key.
 */
std::string HTTPServer::makeHandlersKey(std::string_view const &method, std::string_view const &path) {
  std::string handlersKey;
  handlersKey.reserve(method.size() + 1 + path.size()); // Preallocate memory
  handlersKey.append(method).append(" ").append(path);

  return handlersKey;
}

[[maybe_unused]] void HTTPServer::logHandlers() const {
  if (handlers.empty()) {
    logger << "No query parameters" << std::endl;
  } else {
    logger << "Handlers (" << handlers.size() << "):" << std::endl;
    for (const auto& [path, handler] : handlers) {
      logger << "Path: " << path << ", Handler address: " << &handler << std::endl;
    }
  }
}

std::string HTTPServer::errToString(err_t const err) {
  switch (err) {
    case ERR_OK: return "No error";
    case ERR_MEM: return "Out of memory";
    case ERR_ABRT: return "Connection aborted";
    case ERR_RST: return "Connection reset";
    case ERR_CLSD: return "Connection closed";
    default: return "Unknown error (" + std::to_string(err) + ")";
  }
}

void HTTPServer::onMethod(std::string method, std::string path, HTTPHandler func) {
  std::string const handlersKey = makeHandlersKey(method, path);

  // Store only the last handler for the specified method and path
  handlers[handlersKey] = {std::move(func)};
}

