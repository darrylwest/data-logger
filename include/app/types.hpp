//
// 2025-02-06 19:35:55 dpw
//

#pragma once

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <functional>
#include <iostream>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

using Str = std::string;

using StrView = std::string_view;

using FilePath = std::filesystem::path;

template <typename T> using Vec = std::vector<T>;

template <typename T> using Func = std::function<T>;

template <typename K, typename V> using HashMap = std::unordered_map<K, V>;

template <typename T> using Optional = std::optional<T>;

Str to_uppercase(const std::string& str) {
    std::string result = str;  // Create a copy of the original string
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::toupper(c); });
    return result;
}

Str to_lowercase(const std::string& str) {
    std::string result = str;  // Create a copy of the original string
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}
