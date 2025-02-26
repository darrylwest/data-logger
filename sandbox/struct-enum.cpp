//
// g++ -std=c++20 -Wall -o struct-enum struct-enum.cpp
//
#include <iostream>

struct TaskStatus {
    enum class Value : short {
        Pending,
        InProgress,
        Completed,
        Failed
    };

    static std::string to_label(Value status) {
        switch (status) {
            case Value::Pending:     return "Pending";
            case Value::InProgress:  return "In-Progress";
            case Value::Completed:   return "Completed";
            case Value::Failed:      return "Failed";
            default:          return "Unknown";
        }
    }

    static short to_value(Value status) {
        return static_cast<short>(status);
    }
};

// classic enum
struct ReadingType {
    enum class Value : short {
        Status,
        Temperature,
        Light,
        Humidity,
        Proximity,
        Distance,
    };

    
    static std::string to_label(Value v) {
        switch(v) {
            case Value::Status: return "Status";
            case Value::Temperature: return "Temperature";
            case Value::Light: return "Light";
            case Value::Humidity: return "Humidity";
            case Value::Proximity: return "Proximity";
            case Value::Distance: return "Distance";
            default: return "Unknown";
        }
    }

    static short to_value(Value status) {
        return static_cast<short>(status);
    }
};


int main() {
    TaskStatus::Value status = TaskStatus::Value::InProgress;

    std::cout << "Task is " << TaskStatus::to_label(status) << ", enum: " << TaskStatus::to_value(status) << std::endl;

    ReadingType::Value reading_type = ReadingType::Value::Distance;
    std::cout << "reading: " << ReadingType::to_label(reading_type) << ", enum: " << ReadingType::to_value(reading_type) << std::endl;

    if (reading_type == ReadingType::Value::Distance) {
        std::cout << "reading type is distance, ok.\n";
    } else {
        std::cout << "ERROR! reading type is not distance\n";
    }


    return 0;
}


