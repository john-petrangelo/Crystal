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
  void updateLastActive() { _lastActive = to_ms_since_boot(get_absolute_time()); }

  /**
   * Compares the last active timestamps of two connections.
   * Used to determine the least recently used connection.
   *
   * @param other The other connection context to compare against.
   * @return `true` if this connection is less active than the other; otherwise, `false`.
   */
  bool isLessRecentlyActiveThan(ConnectionContext const& other) const { return _lastActive < other._lastActive; }

  /**
   * Serializes the context into a JSON object.
   *
   * @param obj The JSON object to populate.
   */
  void asJson(JsonObject obj) const;

 /**
  * Increment the number of bytes sent from the response data.
  *
  * @param len The number of bytes sent.
  */
 void didSendBytes(size_t len) { _bytesSent += len; }

 /// Get the unique identifier for this connection.
  uint32_t id() const { return _id; }

  /// Get the pointer to the HTTP _server managing this connection.
  HTTPServer *server() const { return _server; }

  /// Get the pointer to the lwIP TCP PCB for this connection.
  tcp_pcb *pcb() const { return _pcb; }

  /// Get the buffer for incoming request data.
  std::string &requestData() { return _requestData; }

  /// Get the buffer for outgoing response data.
  std::string &responseData() { return _responseData; }

  /// Get the view of the remaining response data to be sent.
  std::string_view remainingResponseData() const { return _remainingResponseData; }

  /// Set the view of the remaining response data to be sent.
  void setRemainingResponseData(std::string_view data) { _remainingResponseData = data; }

  /// Get the number of bytes already sent from the response data.
  size_t bytesSent() const { return _bytesSent; }

  /// Returns a boolean indicating whether the connection should be closed after the response is sent.
  bool shouldCloseAfterResponse() const { return _shouldCloseAfterResponse; }

  /// Set a flag indicating that the connection should be closed after the response is sent.
  void setShouldCloseAfterResponse() { _shouldCloseAfterResponse = true; }

  /// Get the HTTP request _parser for this connection.
  HTTPRequestParser &parser() { return _parser; }

  /// Overload for printing a connection context to an output stream.
  friend std::ostream& operator<<(std::ostream& os, const ConnectionContext& context);

private:
  /// Static counter for generating unique IDs for connections.
  /// Incremented every time a new ConnectionContext is created.
  static uint32_t nextID;

  /// Unique identifier for this connection.
  uint32_t _id;

  /// Pointer to the HTTP _server managing this connection.
  HTTPServer * const _server;

  /// Pointer to the lwIP TCP PCB for this connection. Immutable after initialization.
  tcp_pcb *_pcb; // Pointer to the connection's PCB

  /// Buffer for incoming request data. Stores the raw data read from the connection.
  std::string _requestData;

  /// Buffer for outgoing response data. Contains the serialized HTTP response to be sent.
  std::string _responseData;

  /// View of the remaining response data to be sent. Efficiently tracks the remaining unsent portion of _responseData.
  std::string_view _remainingResponseData;

  /// Number of bytes already sent from `_responseData`.
  size_t _bytesSent;

  /// Flag indicating whether the connection should be closed after the response is sent.
  bool _shouldCloseAfterResponse = false;

  /// HTTP request _parser for this connection.
  HTTPRequestParser _parser;

  /// Timestamp of the last activity on this connection. Updated whenever data is sent or received.
  uint32_t _lastActive = 0;
};
