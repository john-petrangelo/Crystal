#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>

#include <lumos-arduino/Logger.h>

#include "HTTPRequest.h"
#include "HTTPRequestParser.h"
#include "HTTPServer.h"

void HTTPServer::init() {
  auto pcb = tcp_new();
  if (pcb != nullptr) {
    err_t err = tcp_bind(pcb, IP_ADDR_ANY, 80);
    if (err == ERR_OK) {
      pcb = tcp_listen(pcb);
      tcp_arg(pcb, this);
      tcp_accept(pcb, onAccept);
      logger << "HTTP server listening on port 80" << std::endl;
    } else {
      logger << "Error in tcp_bind: " << err << std::endl;
    }
  } else {
    logger << "Error in tcp_new" << std::endl;
  }
}

err_t HTTPServer::onAccept(void *arg, tcp_pcb *newpcb, [[maybe_unused]] err_t err) {
  auto const server = static_cast<HTTPServer*>(arg);
  tcp_arg(newpcb, server);
  tcp_recv(newpcb, onReceive);
  tcp_sent(newpcb, onSent);
  tcp_err(newpcb, onError);
  return ERR_OK;
}

err_t HTTPServer::onReceive(void *arg, tcp_pcb *tpcb, pbuf *p, err_t const err) {
  auto const server = static_cast<HTTPServer*>(arg);
  if (err != ERR_OK) {
    logger <<"Error in onReceive: " <<  err << std::endl;
    server->closeConnection(tpcb); // Close on error
    return err;
  }

  if (p == nullptr) {
    logger << "Connection closed by client" << std::endl;
    server->closeConnection(tpcb); // Close connection if pbuf is NULL
    return ERR_OK;
  }

  // Get the data payload from the supplied buffer
  server->data.append(static_cast<char*>(p->payload), p->len);
  pbuf_free(p);

  logger << "onReceive len=" << server->data.length() << std::endl;
  logger << "=== START OF DATA ===" << std::endl;
  logger << server->data << std::endl;
  logger << "=== END OF DATA ===" << std::endl;

  // Parse the raw request payload
  server->parser.parse(server->data);

  if (HTTP_DEBUG) {
    logger << "=== HTTP Request Start ===" << std::endl;
    server->logHTTPRequest(server->parser.request());
    logger << "=== HTTP Request End ===" << std::endl;
  // } else {
  //   logger << "Request received " << server->request->method << " " << server->request->path << std::endl;
  }

  if (server->parser.state() == HTTPRequestParser::RequestState::FAILED) {
    // TODO Close the connection and reset the parser
    server->sendResponse(tpcb, {400, "Invalid request"});
    return ERR_OK;
  }

  if (server->parser.state() == HTTPRequestParser::RequestState::COMPLETE) {
    logger << "Skipping finding and invoking handler..." << std::endl;
    server->sendResponse(tpcb, {200, "Fake success"});
    server->parser = HTTPRequestParser();

    // Match the method and path to a handler, otherwise return a not found error
    // std::string const handlersKey = makeHandlersKey(server->request->method, server->request->path);
    // auto it = server->handlers.find(handlersKey);
    // if (it != server->handlers.end()) {
    //   // TODO Handlers expect reference, not pointer...
    //   // TODO Should we make a local reference called "reqeust", e.g. HTTPRequest &request = server->request ??
    //   auto const response = it->second(*server->request);  // Call the handler
    //   server->sendResponse(tpcb, response);
    // } else {
    //   server->sendResponse(tpcb, {404, ""});
    // }
  }


  if (HTTP_DEBUG) {
    logger << "Request processed " << server->parser.request().method << " " << server->parser.request().path << std::endl;
  }

  server->sendResponse(tpcb, {200, "Fake success"});
  return ERR_OK; // Return ERR_OK to continue receiving
}

err_t HTTPServer::onSent(void *arg, tcp_pcb *tpcb, u16_t len) {
  auto server = static_cast<HTTPServer*>(arg);
  logger << "Response sent (" << len << "bytes)" << std::endl;
  return ERR_OK;
}

void HTTPServer::onError(void *arg, err_t err) {
  auto server = static_cast<HTTPServer*>(arg);
  logger << "Connection error: " << err << std::endl;
  server->closeConnection(static_cast<struct tcp_pcb*>(arg)); // Handle error
}

void HTTPServer::onGet(const std::string& path, HTTPHandler func) {
  std::string handlersKey = makeHandlersKey("GET", path);

  // Store only the last handler for the specified method and path
  handlers[handlersKey] = {std::move(func)};
}

void HTTPServer::onPut(const std::string &path, HTTPHandler func) {
  std::string handlersKey = makeHandlersKey("PUT", path);

  // Store only the last handler for the specified method and path
  handlers[handlersKey] = {std::move(func)};
}

err_t HTTPServer::sendResponse(tcp_pcb *tpcb, HTTPResponse const &response) {
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

  return sendRawResponse(tpcb, responseStream.str());
}

err_t HTTPServer::sendRawResponse(tcp_pcb *tpcb, std::string const &rawResponse) {
  u16_t len = rawResponse.size();
  if (HTTP_DEBUG) {
    logger << "Sending response, total len=" << rawResponse.size() << std::endl;
  }

  // Ensure the response fits within the TCP buffer
  const char* response_data = rawResponse.c_str();
  while (len > 0) {
    u16_t send_len = tcp_sndbuf(tpcb);
    if (send_len > len) {
      send_len = len;
    }

    if (HTTP_DEBUG) {
      logger << "Sending " << send_len << " bytes..." << std::endl;
    }
    err_t err = tcp_write(tpcb, response_data, send_len, TCP_WRITE_FLAG_COPY);
    if (err != ERR_OK) {
      logger << "Error in tcp_write: " << err << std::endl;
      return err;
    }

    response_data += send_len;
    len -= send_len;

    // Flush the buffer explicitly
    err = tcp_output(tpcb);
    if (err != ERR_OK) {
      logger << "Error in tcp_output: " << err << std::endl;
      return err;
    }
  }

  if (HTTP_DEBUG) {
    logger << "Send complete" << std::endl;
  }
  return ERR_OK;
}

void HTTPServer::closeConnection(tcp_pcb *tpcb) {
  err_t err = tcp_close(tpcb);
  if (err != ERR_OK) {
    logger << "Error in tcp_close: " << err << std::endl;
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

void HTTPServer::logHTTPRequest(HTTPRequest const &request) {
  logger << "Method: " << request.method << std::endl;
  logger << "Path: " << request.path << std::endl;
  if (request.queryParams.empty()) {
    logger << "No query parameters" << std::endl;
  } else {
    logger << "Query params:" << std::endl;
    for (const auto& param : request.queryParams) {
      logger << "  " << param.first << ": " << param.second << std::endl;
    }
  }
  if (request.headers.empty()) {
    logger << "No headers" << std::endl;
  } else {
    logger << "Headers:" << std::endl;
    for (const auto& header : request.headers) {
      logger << "  " << header.first << ": " << header.second << std::endl;
    }
  }

  if (request.body.empty()) {
    logger << "No body" << std::endl;
  } else {
    logger << "Body:" << std::endl;
    logger << request.body << std::endl;
    logger << "=== END OF BODY ===" << std::endl;
  }
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
