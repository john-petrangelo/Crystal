#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

class HTTPRequest {
public:
    std::string method;
    std::string path;
    std::unordered_map<std::string, std::string> queryParams;
    std::unordered_map<std::string, std::string> headers;
    uint32_t contentLength = 0;
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
