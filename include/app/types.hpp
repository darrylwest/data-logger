//
// 2025-02-06 19:35:55 dpw
//

#pragma once

#include <functional>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include <filesystem>

using Str = std::string;

using StrView = std::string_view;

// broken
// using fs = std::filesystem;

template <typename T> using Vec = std::vector<T>;

template <typename T> using Func = std::function<T>;

template <typename K, typename V> using HashMap = std::unordered_map<K, V>;

template <typename T> using Optional = std::optional<T>;
