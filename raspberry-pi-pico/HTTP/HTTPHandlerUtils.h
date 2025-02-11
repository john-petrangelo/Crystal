#pragma once

#include <optional>
#include <string>
#include <unordered_map>

#include <ArduinoJson.h>

#include "lumos-arduino/Colors.h"

class HTTPHandlerUtils {
public:
  static bool parseJsonBody(JsonDocument &doc, std::string const &body, char const *handlerName);

  template<typename T>
  static T getJsonValue(JsonVariantConst const obj, std::string_view const paramName, T defaultValue) {
    auto const value = obj[paramName];
    if (value.isNull() || !value.is<T>()) {
      return defaultValue;
    }

    return value.as<T>();
  }

  static Color getJsonColor(JsonVariant const obj, std::string_view paramName, Color defaultColor = BLACK);
  static std::optional<long> getArgAsLong(std::unordered_map<std::string, std::string> const &queryParams, std::string_view paramName);
  static std::optional<float> getArgAsFloat(std::unordered_map<std::string, std::string> const &queryParams, std::string_view paramName);
  static std::optional<Color> getArgAsColor(std::unordered_map<std::string, std::string> const &queryParams, std::string_view paramName);
};
