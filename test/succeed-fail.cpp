//
// example of how to handle errors
// g++ -std=c++20 -o succeed-fail succeed-fail.cpp
//

#include <iostream>
#include <string>
#include <optional>
#include <nlohmann/json.hpp>


struct AppError {
    std::string message;
    int code;  // For more detailed error codes if needed
};

// Type alias for clarity
template <typename T>
using ResultPair = std::pair<T, std::optional<AppError>>;

// Simulate an HTTP GET request
ResultPair<std::string> http_get(const std::string& url) {
    if (url == "http://fail.com") {
        const auto e = AppError{ .message = "network error", .code = 404 };
        return {"", e};
    }
    return {R"({"id": 1, "name": "Service Data"})", std::nullopt};
}

// Parse JSON into a struct
struct Data {
    int id;
    std::string name;
};

ResultPair<Data> parse_json(const std::string& json_str) {
    try {
        auto json_obj = nlohmann::json::parse(json_str);
        Data data{ json_obj.at("id").get<int>(), json_obj.at("name").get<std::string>() };
        return {data, std::nullopt};
    } catch (nlohmann::json::exception& e) {
        const auto err = AppError{ .message = e.what(), .code = 904 };
        return {{}, err};
    }
}

// Handle service logic
void handle_service(const std::string& url) {
    auto [response, http_err] = http_get(url);
    if (http_err) {
        std::cerr << "HTTP Error: " << http_err->message << std::endl;
        return;
    }

    auto [data, parse_err] = parse_json(response);
    if (parse_err) {
        std::cerr << "Parsing Error: " << parse_err->message << std::endl;
        return;
    }

    std::cout << "Service Success: ID=" << data.id << ", Name=" << data.name << std::endl;
}

int main() {
    handle_service("http://success.com");  // Success case
    handle_service("http://fail.com");     // Failure case
}

