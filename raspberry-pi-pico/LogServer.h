#pragma once

#include <pico/cyw43_arch.h>

class LogServer {
public:
    LogServer() = default;
    ~LogServer() = default;

    void init();
    void sendMessage(struct tcp_pcb *tpcb, char const *msg, size_t msgLen);

private:
    static constexpr int PORT = 8000;

    static err_t onAccept(void *arg, struct tcp_pcb *newpcb, [[maybe_unused]] err_t err);
    static err_t onReceive(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
    static void onError(void *arg, err_t err);

    void closeConnection(struct tcp_pcb *tpcb);
};
