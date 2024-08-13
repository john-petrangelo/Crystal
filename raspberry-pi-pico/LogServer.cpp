#include <cstdio>
#include <string>

#include <lwip/tcp.h>

#include "LogServer.h"

err_t LogServer::onAccept(void *arg, struct tcp_pcb *newpcb, [[maybe_unused]] err_t err) {
  auto server = static_cast<LogServer*>(arg);
  tcp_arg(newpcb, server);
  tcp_recv(newpcb, onReceive);
  tcp_sent(newpcb, onSent);
  tcp_err(newpcb, onError);
  return ERR_OK;
}

err_t LogServer::onReceive(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
  auto server = static_cast<LogServer*>(arg);
  if (err == ERR_OK) {
    if (p != nullptr) {
      // Parse the raw request payload
      char const * data = static_cast<char const *>(p->payload);

      // TODO Do stuff
      // ...
      // For now, just echo
      server->sendMessage(tpcb, data);

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

err_t LogServer::onSent(void *arg, struct tcp_pcb *tpcb, u16_t len) {
  auto server = static_cast<LogServer*>(arg);
  printf("Response sent, closing connection\n");
  server->closeConnection(tpcb); // Close the connection after sending the response
  return ERR_OK;
}

void LogServer::onError(void *arg, err_t err) {
  auto server = static_cast<LogServer*>(arg);
  printf("Connection error: %d\n", err);
  server->closeConnection(static_cast<struct tcp_pcb*>(arg)); // Handle error
}

void LogServer::closeConnection(struct tcp_pcb *tpcb) {
  err_t err = tcp_close(tpcb);
  if (err != ERR_OK) {
    printf("Error in tcp_close: %d\n", err);
  }
}

void LogServer::sendMessage(struct tcp_pcb *tpcb, char const *msg) {
  err_t err;
  u16_t lenRemaining = strlen(msg);  // Use strlen to get the length of the C-string

  // Ensure the response fits within the TCP buffer
  while (lenRemaining > 0) {
    u16_t send_len = tcp_sndbuf(tpcb);
    if (send_len > lenRemaining) {
      send_len = lenRemaining;
    }

    err = tcp_write(tpcb, msg, send_len, TCP_WRITE_FLAG_COPY);
    if (err != ERR_OK) {
      printf("Error in tcp_write: %d\n", err);
    }

    msg += send_len;
    lenRemaining -= send_len;
  }

  // Ensure data is sent
  err = tcp_output(tpcb);
  if (err != ERR_OK) {
    printf("Error in tcp_output: %d\n", err);
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
      printf("Log server listening on port %d\n", PORT);
    } else {
      printf("Error in tcp_bind: %d\n", err);
    }
  } else {
    printf("Error in tcp_new\n");
  }
}

void LogServer::loop() {

}
