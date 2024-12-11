#include <iostream>

#include "HTTPRequest.h"

std::ostream& operator<<(std::ostream& os, const HTTPRequest& request) {
    os << "HTTP Request - " << request.method << " " << request.path << std::endl;

    if (request.queryParams.empty()) {
        os << "No query parameters" << std::endl;
    } else {
        os << "Query params:" << std::endl;
        for (const auto& param : request.queryParams) {
            os << "  " << param.first << ": " << param.second << std::endl;
        }
    }

    if (request.headers.empty()) {
        os << "No headers" << std::endl;
    } else {
        os << request.headers.size() << " headers" << std::endl;

        // os << "Headers:" << std::endl;
        // for (const auto& header : request.headers) {
        //     os << "  " << header.first << ": " << header.second << std::endl;
        // }
    }

    if (request.body.empty()) {
        os << "No body" << std::endl;
    } else {
        os << "Body: <<<" << std::endl;
        os << request.body << std::endl;
        os << ">>> END OF BODY" << std::endl;
    }

    return os;
}
