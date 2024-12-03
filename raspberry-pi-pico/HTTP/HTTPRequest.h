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
};

// #include <iostream>
//
// // In class:
// friend std::ostream& operator<<(std::ostream& os, const HTTPRequest& request);
//
// std::ostream& operator<<(std::ostream& os, const HTTPRequest& request) {
//     // Print the method and path
//     os << "Method: " << request.method << "\n";
//     os << "Path: " << request.path << "\n";
//
//     // Print query parameters
//     os << "Query Parameters:\n";
//     for (const auto& [key, value] : request.queryParams) {
//         os << "  " << key << ": " << value << "\n";
//     }
//
//     // Print headers
//     os << "Headers:\n";
//     for (const auto& [key, value] : request.headers) {
//         os << "  " << key << ": " << value << "\n";
//     }
//
//     return os;
// }
