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

void HTTPServer::init() {
  auto pcb = tcp_new();
  if (pcb == nullptr) {
    logger << "Error calling tcp_new" << std::endl;
  }

  if (err_t const err = tcp_bind(pcb, IP_ADDR_ANY, 80); err != ERR_OK) {
    logger << "Error calling tcp_bind: " << err << std::endl;
  }

  pcb = tcp_listen(pcb);
  tcp_arg(pcb, this);
  tcp_accept(pcb, onAccept);
  logger << "HTTP server listening on port 80" << std::endl;
}

err_t HTTPServer::onAccept(void *arg, tcp_pcb *newpcb, [[maybe_unused]] err_t err) {
  auto *server = static_cast<HTTPServer*>(arg);
  auto *context = new ConnectionContext{server, newpcb};

  if (HTTP_DEBUG) {
    std::string const remoteIpStr = ipaddr_ntoa(&newpcb->remote_ip);
    logger << *context << "Accepted new connection from " << remoteIpStr << ":" << newpcb->remote_port << std::endl;
  }

  tcp_arg(newpcb, context);
  tcp_recv(newpcb, onReceive);
  tcp_sent(newpcb, onSent);
  tcp_err(newpcb, onError);
  return ERR_OK;
}

err_t HTTPServer::onReceive(void *arg, tcp_pcb *tpcb, pbuf *p, err_t const err) {
  auto const context = static_cast<ConnectionContext*>(arg);
  if (err != ERR_OK) {
    logger << *context << "Error in onReceive: " <<  err << std::endl;
    context->server->closeConnection(context); // Close on error
    return err;
  }

  if (p == nullptr) {
    logger << *context << "Connection closed by client" << std::endl;
    context->server->closeConnection(context); // Close connection if pbuf is NULL
    return ERR_OK;
  }

  // Get the data payload from the supplied buffer
  context->data.append(static_cast<char*>(p->payload), p->len);
  pbuf_free(p);

  // Parse the raw request payload
  context->parser.parse(context->data);
  HTTPRequest const &request = context->parser.request();

  if (HTTP_DEBUG) {
    logger << *context << request << std::endl;
  } else {
    logger << *context << "Request received " << request.method << " " << request.path << std::endl;
  }

  if (context->parser.state() == HTTPRequestParser::RequestState::FAILED) {
    // TODO Close the connection and reset the parser
    context->server->sendResponse(context, {400, "Invalid request"});
    return ERR_OK;
  }

  if (context->parser.state() == HTTPRequestParser::RequestState::COMPLETE) {

    // Match the method and path to a handler, otherwise return a not found error
    std::string const handlersKey = makeHandlersKey(request.method, request.path);
    auto it = context->server->handlers.find(handlersKey);
    if (it != context->server->handlers.end()) {
      auto const response = it->second(request);  // Call the handler
      context->server->sendResponse(context, response);
    } else {
      context->server->sendResponse(context, {404, ""});
    }

    // Reset the parser
    context->parser = HTTPRequestParser();
  }

  return ERR_OK; // Return ERR_OK to continue receiving
}

err_t HTTPServer::onSent(void *arg, tcp_pcb *tpcb, u16_t const len) {
  auto const context = static_cast<ConnectionContext*>(arg);
  if (HTTP_DEBUG) {
    logger << *context << "Response sent (" << len << " bytes)" << std::endl;
  }
  return ERR_OK;
}

void HTTPServer::onError(void *arg, err_t err) {
  auto context = static_cast<ConnectionContext*>(arg);
  logger << *context << "Connection error: " << err << std::endl;
  context->server->closeConnection(context); // Handle error
}

void HTTPServer::onGet(const std::string& path, HTTPHandler func) {
  std::string const handlersKey = makeHandlersKey("GET", path);

  // Store only the last handler for the specified method and path
  handlers[handlersKey] = {std::move(func)};
}

void HTTPServer::onPut(const std::string &path, HTTPHandler func) {
  std::string const handlersKey = makeHandlersKey("PUT", path);

  // Store only the last handler for the specified method and path
  handlers[handlersKey] = {std::move(func)};
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

  // Add the response body
  responseStream << response.body;

  return sendRawResponse(context, responseStream.str());
}

err_t HTTPServer::sendRawResponse(ConnectionContext *context, std::string const &rawResponse) {
  u16_t len = rawResponse.size();

  // Ensure the response fits within the TCP buffer
  const char* response_data = rawResponse.c_str();
  while (len > 0) {
    u16_t send_len = tcp_sndbuf(context->pcb);
    if (send_len > len) {
      send_len = len;
    }

    err_t err = tcp_write(context->pcb, response_data, send_len, TCP_WRITE_FLAG_COPY);
    if (err != ERR_OK) {
      logger << *context << "Error in tcp_write: " << err << std::endl;
      return err;
    }

    response_data += send_len;
    len -= send_len;

    // Flush the buffer explicitly
    err = tcp_output(context->pcb);
    if (err != ERR_OK) {
      logger << *context << "Error in tcp_output: " << err << std::endl;
      return err;
    }
  }

  return ERR_OK;
}

void HTTPServer::closeConnection(ConnectionContext *context) {
  if (err_t err = tcp_close(context->pcb); err != ERR_OK) {
    logger << *context << "Error in tcp_close: " << err << std::endl;
  } else {
    logger << *context << "Connection closed" << std::endl;
  }
}

std::string HTTPServer::makeHandlersKey(std::string_view const &method, std::string_view const &path) {
  std::string handlersKey;
  handlersKey.reserve(method.size() + 1 + path.size()); // Preallocate memory
  handlersKey.append(method);
  handlersKey.append(" ");
  handlersKey.append(path);

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
