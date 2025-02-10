//
// g++ -std=c++20 -o retry-service retry-service.cpp
//
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include <exception>

// Exception Classes
class ServiceException : public std::exception {
public:
    explicit ServiceException(const std::string& message) : msg_(message) {}
    const char* what() const noexcept override { return msg_.c_str(); }
private:
    std::string msg_;
};

class NetworkException : public ServiceException {
public:
    using ServiceException::ServiceException;
};

class ParseException : public ServiceException {
public:
    using ServiceException::ServiceException;
};

// Data Structure
struct Data {
    int id;
    std::string name;
};

// HTTP and JSON Functions
std::string http_get(const std::string& url) {
    static int attempt = 0;
    attempt++;
    if (url == "http://unstable.com" && attempt < 3) {  // Simulate failure on first two attempts
        throw NetworkException("Temporary network error on attempt " + std::to_string(attempt));
    }
    return R"({"id": 1, "name": "Reliable Service"})";
}

Data parse_json(const std::string& json_str) {
    try {
        auto json_obj = nlohmann::json::parse(json_str);
        return { json_obj.at("id").get<int>(), json_obj.at("name").get<std::string>() };
    } catch (const std::exception& e) {
        throw ParseException("JSON parsing failed: " + std::string(e.what()));
    }
}

// Retry Mechanism
template <typename Func, typename... Args>
auto retry(Func func, int max_attempts, int base_delay_ms, Args&&... args) {
    int attempt = 0;
    while (attempt < max_attempts) {
        try {
            return func(std::forward<Args>(args)...);  // Attempt the function
        } catch (const NetworkException& e) {
            attempt++;
            spdlog::warn("Attempt {} failed: {}. Retrying...", attempt, e.what());
            if (attempt == max_attempts) throw;  // Rethrow if max attempts reached
            std::this_thread::sleep_for(std::chrono::milliseconds(base_delay_ms * (1 << (attempt - 1))));  // Exponential backoff
        }
    }
    throw ServiceException("Unexpected error in retry logic.");  // Should never reach here
}

// Service Handler
void handle_service(const std::string& url) {
    try {
        std::string response = retry(http_get, 5, 500, url);  // Retry up to 5 times with 500ms base delay
        Data data = parse_json(response);
        spdlog::info("Service Success: ID={}, Name={}", data.id, data.name);
    } catch (const ServiceException& e) {
        spdlog::error("Service Error: {}", e.what());
    } catch (const std::exception& e) {
        spdlog::error("Unexpected Error: {}", e.what());
    }
}

int main() {
    spdlog::set_level(spdlog::level::info);  // Set logging level to info
    handle_service("http://unstable.com");   // Will retry before succeeding
    handle_service("http://always-fail.com"); // Simulate permanent failure by modifying http_get if needed
    return 0;
}

