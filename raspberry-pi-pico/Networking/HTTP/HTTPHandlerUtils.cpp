#include <ArduinoJson.h>

#include "lumos-arduino/Logger.h"

#include "HTTPHandlerUtils.h"

bool HTTPHandlerUtils::parseJsonBody(JsonDocument &doc, std::string const &body, char const *handlerName) {
  DeserializationError const error = deserializeJson(doc, body);
  if (error) {
    logger << handlerName << " failed to parse JSON: " << error.c_str() << std::endl;
    return false;
  }

  return true;
}

Color HTTPHandlerUtils::getJsonColor(JsonVariant const obj, std::string_view const paramName, Color const defaultColor) {
  auto const value = obj[paramName];
  if (value.isNull() || !value.is<const char *>()) {
    return defaultColor;
  }

  return strtol(value.as<char const *>(), nullptr, 16);
}

std::optional<long> HTTPHandlerUtils::getArgAsLong(std::unordered_map<std::string, std::string> const &queryParams, std::string_view const paramName) {
  if (auto const param = queryParams.find(std::string(paramName)); param != queryParams.end()) {
    // Convert the value from string to long
    char *endPtr = nullptr;
    long value = std::strtol(param->second.c_str(), &endPtr, 10);

    // Check if the conversion was successful (i.e., the entire string was a valid number)
    if (endPtr != param->second.c_str()) {
      return value;
    }
  }

  logger << "Missing '" << paramName << "' parameter in request" << std::endl;
  return std::nullopt;
}

std::optional<float> HTTPHandlerUtils::getArgAsFloat(std::unordered_map<std::string, std::string> const &queryParams, std::string_view const paramName) {
  if (auto const param = queryParams.find(std::string(paramName)); param != queryParams.end()) {
    // Convert the value from string to float
    char *endPtr = nullptr;
    float value = std::strtof(static_cast<std::string>(param->second).c_str(), &endPtr);

    // Check if the conversion was successful (i.e., the entire string was a valid number)
    if (endPtr != param->second.c_str()) {
      return value;
    }
  }

  logger << "Missing '" << paramName << "' parameter in request" << std::endl;
  return std::nullopt;
}

std::optional<Color> HTTPHandlerUtils::getArgAsColor(std::unordered_map<std::string, std::string> const &queryParams, std::string_view const paramName) {
  if (auto const param = queryParams.find(std::string(paramName)); param != queryParams.end()) {
    // Convert the value from hex string to Color
    char *endPtr = nullptr;
    Color color = std::strtol(static_cast<std::string>(param->second).c_str(), &endPtr, 16);
    if (endPtr != param->second.c_str()) {
      return color;
    }
  }

  logger << "Missing '" << paramName << "' parameter in request" << std::endl;
  return std::nullopt;
}
