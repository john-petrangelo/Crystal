#include <ostream>

#include "HTTPRequest.h"
#include "HTTPRequestParser.h"

#include <lumos-arduino/Logger.h>
#include "Utils.h"

void HTTPRequestParser::parseFirstLine(std::string &data) {
  // Make sure the first line has a proper linebreak
  auto firstLineEnd = data.find("\r\n");
  if (firstLineEnd == std::string::npos) {
    logger << "Malformed request, first line has no linebreak" << std::endl;
    _state = RequestState::FAILED;
    return;
  }
  auto firstLineParts = split(data.substr(0, firstLineEnd), ' ');
  if (firstLineParts.size() != 3) {
    logger << "Malformed request, first line expected 3 parts, found " << firstLineParts.size() << std::endl;
    _state = RequestState::FAILED;
    return;
  }

  _request.method = firstLineParts[0];
  auto path = firstLineParts[1];
  // The third part is supposed to be HTTP/1.1, don't bother validating

  // The path may include query parameters, e.g. /color?red=0.0&green=0.5&blue=0.5
  size_t const queryStart = path.find('?');
  if (queryStart == std::string::npos) {
    // No query parameters, just set the path
    _request.path = path;
  } else {
    // There are query parameters, first extract the path
    _request.path = path.substr(0, queryStart);

    // Now extract the query parameters
    auto const query = path.substr(queryStart + 1, path.length() - queryStart - 1);
    size_t paramStart = 0;
    while (paramStart < query.length()) {
      size_t paramEnd = query.find('&', paramStart);
      if (paramEnd == std::string::npos) {
        paramEnd = query.length();
      }
      auto const equalPos = query.find('=', paramStart);
      if (equalPos != std::string::npos && equalPos < paramEnd) {
        std::string_view const key = query.substr(paramStart, equalPos - paramStart);
        std::string_view const value = query.substr(equalPos + 1, paramEnd - equalPos - 1);
        _request.queryParams.emplace(trim(key), trim(value));
      }
      paramStart = paramEnd + 1;
    }
  }

  // We're done looking for the first line
  _state = RequestState::RECEIVING_HEADERS;
  data.erase(0, firstLineEnd+2);
}

void HTTPRequestParser::parseHeaders(std::string &data) {
  // We'll start looking from the start of data
  std::string::size_type headerStart = 0;

  // Keep going until there are no more headers
  while (headerStart < data.length()) {
    // Find the end of line for the next header
    size_t const headerEnd = data.find("\r\n", headerStart);

    if (headerEnd == std::string::npos) {
      // No end of line, abort and wait to receive more data and then try again
      break;
    }

    if (headerEnd == headerStart) {
      // An "empty" header means we're done with the headers
      _state = RequestState::RECEIVING_BODY;
      break;
    }

    auto const header = std::string_view(data.data() + headerStart, headerEnd - headerStart);
    auto const colonPos = header.find(':');
    if (colonPos == std::string::npos) {
      // No colon found within the header, fail the request
      logger << "Malformed request, header line contains no colon: " << header << std::endl;
      _state = RequestState::FAILED;
      return;
    }

    auto const key = header.substr(0, colonPos);
    auto const value = header.substr(colonPos + 1);
    _request.headers.emplace(trim(key), trim(value));
    logger << "Added header -- " << key << ": " << value << " " << std::endl;

    // Continue on to the next line
    headerStart = headerEnd + 2;
  }

  // Remove the headers that we found and processed from data
//  data.erase(0, headerStart);
}

void HTTPRequestParser::parse(std::string& data) {
  // If we haven't started a request yet, then first extract the method, path, and HTTP version from the first line.
  if (_state == RequestState::IDLE) {
    parseFirstLine(data);
  }

  if (_state == RequestState::RECEIVING_HEADERS) {
    parseHeaders(data);
  }

  if (_state == RequestState::RECEIVING_BODY) {
    // TODO To be implemented, for now just ignore the body for now and assume complete
    _state = RequestState::COMPLETE;
    data.erase();
  }
}

void HTTPRequestParser::reset() {
  _request = HTTPRequest();
  _state = RequestState::IDLE;
}
