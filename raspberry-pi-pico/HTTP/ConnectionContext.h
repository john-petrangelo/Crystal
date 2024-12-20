#pragma once

#include "HTTPRequestParser.h"
#include "HTTPServer.h"

/**
 * Represents the context of a single HTTP connection.
 * Tracks state, manages data buffers, and provides utility methods for JSON output and comparison.
 */
struct ConnectionContext {
  /**
   * Constructs a new ConnectionContext.
   * Associates the connection with the specified server and PCB (Protocol Control Block),
   * and adds it to the server's active connections.
   *
   * @param server Pointer to the HTTP server managing this connection.
   * @param pcb Pointer to the lwIP TCP PCB for this connection.
   */
  explicit ConnectionContext(HTTPServer *server, tcp_pcb *pcb);

  /**
   * Destroys the ConnectionContext.
   * Automatically removes the connection from the server's active connections.
   */
  ~ConnectionContext();

  /**
   * Resets the context for reuse with the same connection.
   * Clears buffers and resets parser state, while keeping the connection PCB intact.
   */
  void reset();

  /**
   * Updates the last activity timestamp to the current time.
   * This is typically called whenever data is read from or written to the connection.
   */
  void updateLastActive() { lastActive = to_ms_since_boot(get_absolute_time()); }

  /**
   * Compares the last active timestamps of two connections.
   * Used to determine the least recently used connection.
   *
   * @param other The other connection context to compare against.
   * @return `true` if this connection is less active than the other; otherwise, `false`.
   */
  bool isLessRecentlyActiveThan(ConnectionContext const& other) const { return lastActive < other.lastActive; }

  /**
   * Serializes the context into a JSON object.
   *
   * @param obj The JSON object to populate.
   */
  void asJson(JsonObject obj) const;

  uint32_t id;
  HTTPServer * const server; // Pointer to the server instance
  tcp_pcb *pcb; // Pointer to the connection's PCB

  std::string inData;
  std::string outData;
  std::string_view remainingOutData;
  size_t bytesSent;

  HTTPRequestParser parser;

  friend std::ostream& operator<<(std::ostream& os, const ConnectionContext& context);

private:
  // The nextID will be used and incremented every time we open a new connection
  static uint32_t nextID;

  // Timestamp that will be updated every time we send or receive data with this connection
  uint32_t lastActive = 0;
};
