#pragma once

class HTTPRequestParser {
public:
    enum class RequestState {
        IDLE,
        RECEIVING_HEADERS,
        RECEIVING_BODY,
        COMPLETE,
        FAILED,
    };

    void parseFirstLine(std::string &data);

    void parseHeaders(std::string &data);

    void parse(std::string& data);

    HTTPRequest request() const { return _request; }
    RequestState state() const { return _state; }

private:
    HTTPRequest _request;
    RequestState _state = RequestState::IDLE;
};
