#include <ranges>
#include <vector>
#include <map>
#include <string>
#include <ctime>
#include <ranges>
#include <string_view>
#include <spdlog/spdlog.h>


struct TimeValue {
    std::time_t ts;
    float tempC;
    float tempF;
    std::string location;
};

auto transform_map_data(const std::map<std::string, std::string>& input_map) {
    std::vector<TimeValue> result;
    result.reserve(input_map.size());

    auto view = input_map 
        | std::views::transform([](const auto& pair) {
            const auto& [key, tempC] = pair;
            
            // Find the first and last dots to extract all parts
            auto first_dot = key.find('.');
            if (first_dot == std::string::npos) 
                throw std::runtime_error("bad format");

            try {
                std::time_t ts = std::stoll(key.substr(0, first_dot));
                float float_tempC = std::stof(tempC);
                float tempF = float_tempC * 1.8 + 32;
                // Extract everything after the first dot for the location
                std::string location = key.substr(first_dot + 1);
                
                return TimeValue{ts, float_tempC, tempF, location};
            } catch (const std::exception&) {
                throw;
            }
        })
        | std::views::filter([](const TimeValue& tv) {
            return tv.ts != 0; // Filter out invalid entries
        });

    std::ranges::copy(view, std::back_inserter(result));
    return result;
}

int main() {
    std::map<std::string, std::string> temperatures = {
        { "1740420600.cottage.0", "15.000000" },
        { "1740420900.cottage.0", "15.109380" },
        { "1740421200.cottage.0", "15.062500" },
        { "1740421500.cottage.0", "15.234380" },
        { "1740421800.cottage.0", "15.567710" },
        { "1740422100.cottage.0", "15.411460" },
        { "1740422400.cottage.0", "15.343750" },
        { "1740422700.cottage.0", "15.458330" },
        { "1740423000.cottage.0", "15.651040" },
        { "1740423300.cottage.0", "15.739580" },
        { "1740423600.cottage.0", "16.010420" },
        { "1740423900.cottage.0", "16.192711" },
        { "1740424200.cottage.0", "16.244789" },
        { "1740424500.cottage.0", "16.062500" },
        { "1740424800.cottage.0", "16.041670" },
        { "1740425100.cottage.0", "15.833330" },
        { "1740425400.cottage.0", "15.812500" },
        { "1740425700.cottage.0", "16.234381" },
        { "1740426000.cottage.0", "16.765631" },
        { "1740426300.cottage.0", "16.484381" },
        { "1740426600.cottage.0", "16.666670" },
        { "1740426900.cottage.0", "16.609381" },
        { "1740427200.cottage.0", "16.463539" },
        { "1740427500.cottage.0", "16.567711" },
        { "1740427800.cottage.0", "16.906250" }
    };

    // Transform the data
    auto transformed_data = transform_map_data(temperatures);

    // Use the transformed data
    for (const auto& tdata : transformed_data) {
        // Convert ts to human-readable format (optional)
        // std::string time_str = std::ctime(&ts);
        spdlog::info("{} {:.5f} {:.5f} {}", tdata.ts, tdata.tempC, tdata.tempF, tdata.location);
    }
}
