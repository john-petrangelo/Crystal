#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>

#include "HTTPServer.h"

const std::string HTTPServer::httpResponseGet =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 13\r\n"
        "\r\n"
        "Hello, World!";

const std::string HTTPServer::httpResponsePut =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 8\r\n"
        "\r\n"
        "Put Done";

void HTTPServer::init() {
  auto pcb = tcp_new();
  if (pcb != nullptr) {
    err_t err = tcp_bind(pcb, IP_ADDR_ANY, 80);
    if (err == ERR_OK) {
      pcb = tcp_listen(pcb);
      tcp_arg(pcb, this);
      tcp_accept(pcb, onAccept);
      printf("HTTP server listening on port 80\n");
    } else {
      printf("Error in tcp_bind: %d\n", err);
    }
  } else {
    printf("Error in tcp_new\n");
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

HTTPRequest HTTPServer::parseRequest(const std::string& requestStr) {
  HTTPRequest request;
  std::string_view const requestView(requestStr);

  // First extract the method
  size_t const methodEnd = requestView.find(' ');
  request.method = requestView.substr(0, methodEnd);

  // Next extract the path, which may or may not include query parameters
  size_t const pathEnd = requestView.find(' ', methodEnd + 1);
  size_t const queryStart = requestView.find('?', methodEnd + 1);

  if (queryStart != std::string_view::npos && queryStart < pathEnd) {
    // There are query parameters, extract the path first
    request.path = requestView.substr(methodEnd + 1, queryStart - methodEnd - 1);

    // Now extract the query parameters
    auto query = requestView.substr(queryStart + 1, pathEnd - queryStart - 1);

    size_t paramStart = 0;
    while (paramStart < query.length()) {
      size_t paramEnd = query.find('&', paramStart);
      if (paramEnd == std::string_view::npos) {
        paramEnd = query.length();
      }
      auto equalPos = query.find('=', paramStart);
      if (equalPos != std::string_view::npos && equalPos < paramEnd) {
        std::string_view const key = query.substr(paramStart, equalPos - paramStart);
        std::string_view const value = query.substr(equalPos + 1, paramEnd - equalPos - 1);
        request.queryParams.emplace(trim(key), trim(value));
      }
      paramStart = paramEnd + 1;
    }

  } else {
    // No query parameters, just set the path
    request.path = requestView.substr(methodEnd + 1, pathEnd - methodEnd - 1);
  }

  // Parse headers
  size_t headerStart = requestView.find("\r\n", pathEnd) + 2;
  while (headerStart < requestView.length()) {
    size_t headerEnd = requestView.find("\r\n", headerStart);
    if (headerEnd == std::string_view::npos) {
      break;
    }
    size_t colonPos = requestView.find(':', headerStart);
    if (colonPos != std::string_view::npos && colonPos < headerEnd) {
      auto const key = requestView.substr(headerStart, colonPos - headerStart);
      auto const value = requestView.substr(colonPos + 1, headerEnd - colonPos - 1);
      request.headers.emplace(trim(key), trim(value));
    }
    headerStart = headerEnd + 2;
  }

  logHTTPRequest(request);

  return request;
}

void HTTPServer::logHTTPRequest(HTTPRequest const &request) {
  std::cout << "Method: " << request.method << std::endl;
  std::cout << "Path: " << request.path << std::endl;
  if (request.queryParams.empty()) {
    std::cout << "No query parameters" << std::endl;
  } else {
    std::cout << "Query params:" << std::endl;
    for (const auto& param : request.queryParams) {
      std::cout << "  " << param.first << ": " << param.second << std::endl;
    }
  }
  if (request.headers.empty()) {
    std::cout << "No headers" << std::endl;
  } else {
    std::cout << "Headers:" << std::endl;
    for (const auto& header : request.headers) {
      std::cout << "  " << header.first << ": " << header.second << std::endl;
    }
  }
}

err_t HTTPServer::onReceive(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
  auto server = static_cast<HTTPServer*>(arg);
  if (err == ERR_OK) {
    if (p != nullptr) {
      std::string data(static_cast<char*>(p->payload));
      auto request = parseRequest(data);

      // Process the request and send a response
      if (request.method == "GET") {
        auto it = server->onGetHandlers.find(request.path);
        if (it != server->onGetHandlers.end()) {
          auto response = it->second(request);  // Call the handler
        }
        server->sendResponse(tpcb, httpResponseGet);
      } else if (request.method == "PUT") {
        server->sendResponse(tpcb, httpResponsePut);
      } else {
        server->sendResponse(tpcb, "HTTP/1.1 400 Bad Request\r\n\r\n");
      }
      // TODO Create an easier way to send a response using HTTPResponse object
      // TODO Send 404 if path not found

      pbuf_free(p); // Free the pbuf after processing
      printf("Request processed\n");
      return ERR_OK; // Return ERR_OK to continue receiving
    } else {
      printf("Connection closed by client\n");
      server->closeConnection(tpcb); // Close connection if pbuf is NULL
      return ERR_OK;
    }
  } else {
    printf("Error in onReceive: %d\n", err);
    server->closeConnection(tpcb); // Close on error
    return err;
  }
}

err_t HTTPServer::onSent(void *arg, struct tcp_pcb *tpcb, u16_t len) {
  auto server = static_cast<HTTPServer*>(arg);
  printf("Response sent, closing connection\n");
  server->closeConnection(tpcb); // Close the connection after sending the response
  return ERR_OK;
}

void HTTPServer::onError(void *arg, err_t err) {
  auto server = static_cast<HTTPServer*>(arg);
  printf("Connection error: %d\n", err);
  server->closeConnection(static_cast<struct tcp_pcb*>(arg)); // Handle error
}

void HTTPServer::onGet(const std::string& path, HTTPHandler func) {
  // Store only the last handler for the specified path
  onGetHandlers[path] = {std::move(func)};
}

void HTTPServer::onPut(const std::string &path, HTTPHandler func) {
  // Store only the last handler for the specified path
//  onPutHandlers[path] = {std::move(func)};
}

err_t HTTPServer::sendResponse(struct tcp_pcb *tpcb, const std::string &response) {
  err_t err;
  u16_t len = response.size();
  printf("Sending response: %s\n", response.c_str());

  // Ensure the response fits within the TCP buffer
  const char* response_data = response.c_str();
  while (len > 0) {
    u16_t send_len = tcp_sndbuf(tpcb);
    if (send_len > len) {
      send_len = len;
    }

    err = tcp_write(tpcb, response_data, send_len, TCP_WRITE_FLAG_COPY);
    if (err != ERR_OK) {
      printf("Error in tcp_write: %d\n", err);
      return err;
    }

    response_data += send_len;
    len -= send_len;
  }

  // Ensure data is sent
  err = tcp_output(tpcb);
  if (err != ERR_OK) {
    printf("Error in tcp_output: %d\n", err);
  }
  return err;
}

void HTTPServer::closeConnection(struct tcp_pcb *tpcb) {
  err_t err = tcp_close(tpcb);
  if (err != ERR_OK) {
    printf("Error in tcp_close: %d\n", err);
  }
}
