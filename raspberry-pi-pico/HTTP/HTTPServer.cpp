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

err_t HTTPServer::onAccept(void *arg, struct tcp_pcb *newpcb, [[maybe_unused]] err_t err) {
  auto server = static_cast<HTTPServer*>(arg);
  tcp_arg(newpcb, server);
  tcp_recv(newpcb, onReceive);
  tcp_sent(newpcb, onSent);
  tcp_err(newpcb, onError);
  return ERR_OK;
}

err_t HTTPServer::onReceive(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
  auto server = static_cast<HTTPServer*>(arg);
  if (err == ERR_OK) {
    if (p != nullptr) {
      // Parse the raw request payload
      std::string_view data(static_cast<char*>(p->payload));
      auto request = HTTPRequestParser::parse(data);
      logHTTPRequest(request);

      // Match the method and path to a handler, otherwise return a not found error
      std::string handlersKey = makeHandlersKey(request.method, request.path);
      auto it = server->handlers.find(handlersKey);
      if (it != server->handlers.end()) {
        auto response = it->second(request);  // Call the handler
        server->sendResponse(tpcb, response);
      } else {
        server->sendResponse(tpcb, {404, ""});
      }

      pbuf_free(p); // Free the pbuf after processing
      logger << "Request processed" << std::endl;
      return ERR_OK; // Return ERR_OK to continue receiving
    } else {
      logger << "Connection closed by client" << std::endl;
      server->closeConnection(tpcb); // Close connection if pbuf is NULL
      return ERR_OK;
    }
  } else {
    logger <<"Error in onReceive: " <<  err << std::endl;
    server->closeConnection(tpcb); // Close on error
    return err;
  }
}

err_t HTTPServer::onSent(void *arg, struct tcp_pcb *tpcb, u16_t len) {
  auto server = static_cast<HTTPServer*>(arg);
  logger << "Response sent, closing connection" << std::endl;
  server->closeConnection(tpcb); // Close the connection after sending the response
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

err_t HTTPServer::sendResponse(struct tcp_pcb *tpcb, HTTPResponse const& response) {
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
  responseStream << "Content-Type: text/plain\r\n";
  responseStream << "Content-Length: " << response.body.size() << "\r\n";
  responseStream << "\r\n";

  // Add the response body
  responseStream << response.body;

  return sendRawResponse(tpcb, responseStream.str());
}

err_t HTTPServer::sendRawResponse(struct tcp_pcb *tpcb, const std::string &rawResponse) {
  err_t err;
  u16_t len = rawResponse.size();
  logger << "Sending response: " << rawResponse.c_str() << std::endl;

  // Ensure the response fits within the TCP buffer
  const char* response_data = rawResponse.c_str();
  while (len > 0) {
    u16_t send_len = tcp_sndbuf(tpcb);
    if (send_len > len) {
      send_len = len;
    }

    err = tcp_write(tpcb, response_data, send_len, TCP_WRITE_FLAG_COPY);
    if (err != ERR_OK) {
      logger << "Error in tcp_write: " << err << std::endl;
      return err;
    }

    response_data += send_len;
    len -= send_len;
  }

  // Ensure data is sent
  err = tcp_output(tpcb);
  if (err != ERR_OK) {
    logger << "Error in tcp_output: " << err << std::endl;
  }
  return err;
}

void HTTPServer::closeConnection(struct tcp_pcb *tpcb) {
  err_t err = tcp_close(tpcb);
  if (err != ERR_OK) {
    logger << "Error in tcp_close: " << err << std::endl;
  }
}

std::string HTTPServer::makeHandlersKey(std::string_view method, std::string_view path) {
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
}

[[maybe_unused]] void HTTPServer::logHandlers(const std::unordered_map<std::string, HTTPHandler>& handlers) {
  if (handlers.empty()) {
    logger << "No query parameters" << std::endl;
  } else {
    logger << "Handlers (" << handlers.size() << "):" << std::endl;
    for (const auto& [path, handler] : handlers) {
      logger << "Path: " << path << ", Handler address: " << &handler << std::endl;
    }
  }
}
