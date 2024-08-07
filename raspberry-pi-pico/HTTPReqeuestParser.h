#pragma once

class HTTPRequestParser {
public:
    static HTTPRequest parse(const std::string& requestStr);

private:
    static inline std::string_view trim(const std::string_view &str) {
      size_t const start = str.find_first_not_of(" \t\r\n");
      size_t const end = str.find_last_not_of(" \t\r\n");
      return (start == std::string_view::npos) ? "" : str.substr(start, end - start + 1);
    }
};
