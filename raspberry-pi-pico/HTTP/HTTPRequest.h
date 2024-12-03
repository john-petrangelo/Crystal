#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

#include "Utils.h"

class HTTPRequest {
public:
    using QueryMap = std::unordered_map<std::string, std::string>;
    using HeaderMap = std::unordered_map<std::string, std::string, CaseInsensitiveHash, CaseInsensitiveEqual>;

    std::string method;
    std::string path;
    QueryMap queryParams;
    HeaderMap headers;
    uint32_t contentLength = 0;
    std::string body;

    friend std::ostream& operator<<(std::ostream& os, const HTTPRequest& request);
};
