#pragma once

#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>

std::string msToString(uint32_t timeMS);
std::string_view trim(std::string_view const& str);
std::vector<std::string> split(std::string_view str, char delimiter);

// Computes a hash value for a string in a case-insensitive manner.
// This is achieved by converting the string to lowercase before applying the standard hash.
// Useful for creating case-insensitive unordered sets.
struct CaseInsensitiveHash {
  size_t operator()(const std::string &key) const {
    std::string lowerKey = key;
    std::ranges::transform(lowerKey, lowerKey.begin(),
                           [](unsigned char c) { return std::tolower(c); });
    return std::hash<std::string>()(lowerKey);
  }
};

// CaseInsensitiveEqual: Compares two strings for equality in a case-insensitive manner.
// This is achieved by converting each character to lowercase during comparison.
// Useful for creating case-insensitive unordered sets.
struct CaseInsensitiveEqual {
  bool operator()(const std::string &lhs, const std::string &rhs) const {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(),
                      [](unsigned char c1, unsigned char c2) {
                          return std::tolower(c1) == std::tolower(c2);
                      });
  }
};
