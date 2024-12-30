#pragma once

#include <string>

struct HTTPResponse {
    int status_code;
    std::string contentType;
    std::string body;
};
