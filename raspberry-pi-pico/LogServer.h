#pragma once

#include <string>

#include <pico/cyw43_arch.h>

class LogServer {
public:
    LogServer() = default; // Default constructor
    ~LogServer() = default; // Default destructor

    void init();
    void loop();
    void sendMessage(struct tcp_pcb *tpcb, char const *msg);

private:
    static int const PORT = 8000;

    static err_t onAccept(void *arg, struct tcp_pcb *newpcb, [[maybe_unused]] err_t err);
    static err_t onReceive(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
    static err_t onSent(void *arg, struct tcp_pcb *tpcb, u16_t len);
    static void onError(void *arg, err_t err);

    void closeConnection(struct tcp_pcb *tpcb);
};
