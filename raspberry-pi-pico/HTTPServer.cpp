#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>

#include "HTTPRequest.h"
#include "HTTPReqeuestParser.h"
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

err_t HTTPServer::onReceive(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
  auto server = static_cast<HTTPServer*>(arg);
  if (err == ERR_OK) {
    if (p != nullptr) {
      std::string data(static_cast<char*>(p->payload));
      auto request = HTTPRequestParser::parse(data);
      logHTTPRequest(request);

      // Process the request and send a response
      if (request.method == "GET") {
        auto it = server->onGetHandlers.find(std::string(request.path));
        if (it != server->onGetHandlers.end()) {
          auto response = it->second(request);  // Call the handler
          server->sendResponse(tpcb, response);
        } else {
          server->sendResponse(tpcb, {404, ""});
        }
      } else if (request.method == "PUT") {
        server->sendRawResponse(tpcb, httpResponsePut);
      } else {
        server->sendRawResponse(tpcb, "HTTP/1.1 400 Bad Request\r\n\r\n");
      }

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
  printf("Sending response: %s\n", rawResponse.c_str());

  // Ensure the response fits within the TCP buffer
  const char* response_data = rawResponse.c_str();
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

[[maybe_unused]] void HTTPServer::logHandlers(const std::unordered_map<std::string, HTTPHandler>& handlers) {
  if (handlers.empty()) {
    std::cout << "No query parameters" << std::endl;
  } else {
    std::cout << "Handlers (" << handlers.size() << "):" << std::endl;
    for (const auto& [path, handler] : handlers) {
      std::cout << "Path: " << path << ", Handler address: " << &handler << std::endl;
    }
  }
}
