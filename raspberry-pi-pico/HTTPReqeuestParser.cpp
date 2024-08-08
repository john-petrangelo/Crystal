#include "HTTPRequest.h"
#include "HTTPReqeuestParser.h"

HTTPRequest HTTPRequestParser::parse(const std::string_view& requestStr) {
  HTTPRequest request;
  std::string_view const requestView(requestStr);

  // First extract the method
  size_t const methodEnd = requestView.find(' ');
  request.method = requestView.substr(0, methodEnd);

  // Next extract the path, which may or may not include query parameters
  size_t const pathEnd = requestView.find(' ', methodEnd + 1);
  size_t const queryStart = requestView.find('?', methodEnd + 1);

  if (queryStart != std::string_view::npos && queryStart < pathEnd) {
    // There are query parameters, extract the path first
    request.path = requestView.substr(methodEnd + 1, queryStart - methodEnd - 1);

    // Now extract the query parameters
    auto const query = requestView.substr(queryStart + 1, pathEnd - queryStart - 1);

    size_t paramStart = 0;
    while (paramStart < query.length()) {
      size_t paramEnd = query.find('&', paramStart);
      if (paramEnd == std::string_view::npos) {
        paramEnd = query.length();
      }
      auto const equalPos = query.find('=', paramStart);
      if (equalPos != std::string_view::npos && equalPos < paramEnd) {
        std::string_view const key = query.substr(paramStart, equalPos - paramStart);
        std::string_view const value = query.substr(equalPos + 1, paramEnd - equalPos - 1);
        request.queryParams.emplace(trim(key), trim(value));
      }
      paramStart = paramEnd + 1;
    }

  } else {
    // No query parameters, just set the path
    request.path = requestView.substr(methodEnd + 1, pathEnd - methodEnd - 1);
  }

  // Parse headers
  size_t headerStart = requestView.find("\r\n", pathEnd) + 2;
  while (headerStart < requestView.length()) {
    size_t const headerEnd = requestView.find("\r\n", headerStart);
    if (headerEnd == std::string_view::npos) {
      break;
    }
    size_t const colonPos = requestView.find(':', headerStart);
    if (colonPos != std::string_view::npos && colonPos < headerEnd) {
      auto const key  = requestView.substr(headerStart, colonPos - headerStart);
      auto const value = requestView.substr(colonPos + 1, headerEnd - colonPos - 1);
      request.headers.emplace(trim(key), trim(value));
    }
    headerStart = headerEnd + 2;
  }

  return request;
}
