#include <cstdio>
#include <string>

#include <lwip/tcp.h>

#include <lumos-arduino/Logger.h>

#include "LogServer.h"

#include "ConsoleLogger.h"
#include "NetworkLogger.h"

err_t LogServer::onAccept(void *arg, struct tcp_pcb *newpcb, [[maybe_unused]] err_t err) {
  auto server = static_cast<LogServer*>(arg);
  tcp_arg(newpcb, server);
  tcp_recv(newpcb, onReceive);
  tcp_sent(newpcb, nullptr);
  tcp_err(newpcb, onError);

  logger << "Switching to network logger" << std::endl;
  logger.set(new NetworkLogger(newpcb, *server));
  logger << "Switched to network logger" << std::endl;

  return ERR_OK;
}

err_t LogServer::onReceive(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
  auto server = static_cast<LogServer*>(arg);
  if (err == ERR_OK) {
    if (p != nullptr) {
      std::cout << "LogServer received " << p->tot_len << "bytes, ignored" << std::endl;

      pbuf_free(p); // Free the pbuf after processing
      return ERR_OK; // Return ERR_OK to continue receiving
    } else {
      std::cout << "Connection closed by client" << std::endl;
      server->closeConnection(tpcb); // Close connection if pbuf is NULL
      return ERR_OK;
    }
  } else {
    std::cout << "Error in onReceive: " << err << std::endl;
    server->closeConnection(tpcb); // Close on error
    return err;
  }
}

void LogServer::onError(void *arg, err_t err) {
  auto server = static_cast<LogServer*>(arg);
  std::cout << "Connection error: " << err << std::endl;
  server->closeConnection(static_cast<struct tcp_pcb*>(arg)); // Handle error
}

void LogServer::closeConnection(struct tcp_pcb *tpcb) {
  err_t err = tcp_close(tpcb);
  if (err != ERR_OK) {
    std::cout << "Error in tcp_close: " << std::endl;
  }

  Logger::set(new ConsoleLogger());
  logger << "Switched to console logger" << std::endl;
}

void LogServer::sendMessage(struct tcp_pcb *tpcb, char const *msg, size_t const msgLen) {
  err_t err;
  auto lenRemaining = msgLen;

  // Ensure the response fits within the TCP buffer
  while (lenRemaining > 0) {
    u16_t send_len = tcp_sndbuf(tpcb);
    if (send_len > lenRemaining) {
      send_len = lenRemaining;
    }

    auto offset = msgLen - lenRemaining;
    err = tcp_write(tpcb, msg + offset, send_len, TCP_WRITE_FLAG_COPY);
    if (err != ERR_OK) {
      std::cout << "Error in tcp_write: " << err << std::endl;
    }

    // msg += send_len;
    lenRemaining -= send_len;
  }

  // Ensure data is sent
  err = tcp_output(tpcb);
  if (err != ERR_OK) {
    std::cout << "Error in tcp_output: " << err << std::endl;
  }
}

void LogServer::init() {
  auto pcb = tcp_new();

  if (pcb != nullptr) {
    err_t err = tcp_bind(pcb, IP_ADDR_ANY, PORT);
    if (err == ERR_OK) {
      pcb = tcp_listen(pcb);
      tcp_arg(pcb, this);
      tcp_accept(pcb, onAccept);
      logger << "Log server listening on port " << PORT << std::endl;
    } else {
      logger << "Error in tcp_bind: " << err << std::endl;
    }
  } else {
    logger << "Error in tcp_new" << std::endl;
  }
}
