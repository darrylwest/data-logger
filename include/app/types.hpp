//
// 2025-02-06 19:35:55 dpw
//

#pragma once

#include <string>
#include <unordered_map>
#include <vector>

using Str = std::string;

template <typename T> using Vec = std::vector<T>;

template <typename K, typename V> using HashMap = std::unordered_map<K, V>;
