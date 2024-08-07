#pragma once

#include <string>
#include <unordered_map>

class HTTPRequest {
public:
    std::string_view method;
    std::string_view path;
    std::unordered_map<std::string_view, std::string_view> queryParams;
    std::unordered_map<std::string_view, std::string_view> headers;
};
