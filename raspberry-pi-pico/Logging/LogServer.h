#pragma once

#include <lwip/tcp.h>
#include <pico/cyw43_arch.h>

class LogServer {
public:
    LogServer() = default;
    ~LogServer() = default;

    void init();

    static void sendMessage(tcp_pcb *tpcb, char const *msg, size_t msgLen);

private:
    static constexpr int PORT = 8000;

    static err_t onAccept(void *arg, tcp_pcb *newpcb, [[maybe_unused]] err_t err);
    static err_t onReceive(void *arg, tcp_pcb *tpcb, pbuf *p, err_t err);
    static void onError(void *arg, err_t err);

    static void closeConnection(tcp_pcb *tpcb);
};
