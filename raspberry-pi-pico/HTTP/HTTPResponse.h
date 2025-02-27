#pragma once

#include <string>

class HTTPResponse {
public:
    int status_code;
    std::string contentType;
    std::string body;
};
