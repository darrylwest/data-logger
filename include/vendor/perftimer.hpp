//
// A general purpose performance timer in a header-only library with delayed logging.
//

#pragma once

#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <string>

namespace perftimer {
    // Version of the performance timer library
    constexpr auto VERSION = "0.6.4";

    // Type alias for high-resolution clock and time point
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = std::chrono::time_point<Clock>;

    // Performance Timer class to measure execution time
    struct PerfTimer {
    private:
        // Constants for time conversions
        constexpr static auto billions = 1'000'000'000; // 1 billion nanoseconds
        constexpr static auto millions = 1'000'000;      // 1 million nanoseconds

        TimePoint t0; // Start time point
        TimePoint t1; // End time point

        std::string name; // Name of the timer for identification

    public:
        // Constructor that initializes the timer with a unique name
        PerfTimer(const std::string timer_name) : name(timer_name) { };

        // Output stream for buffered logging; allows delayed console output
        std::ostringstream log;

        // Precision for displaying time; defaults to 9 decimal places
        int prec = 9;

        // Start or restart the timer; call stop() to capture the duration
        void start() {
            t0 = Clock::now(); // Record the current time as start time
        }

        // Stop the timer and record the end time
        void stop() {
            t1 = Clock::now(); // Record the current time as end time
        }

        // Calculate and return the duration in nanoseconds between start and end
        auto get_duration() {
            const std::chrono::duration<double, std::nano> dur = t1 - t0; // Calculate duration
            return dur; // Return duration
        }

        // Display the duration with a custom message
        void show_duration(const std::string& message = ": process took: ") {
            auto dur = get_duration(); // Get the duration
            // Output the duration in appropriate units
            if (dur.count() > billions) {
                std::cout << name << message << std::setprecision(prec) << dur.count() / billions << " seconds" << '\n';
            } else if (dur.count() > millions) {
                std::cout << name << message << std::setprecision(prec) << dur.count() / millions << " milliseconds" << '\n';
            } else {
                std::cout << name << message << std::setprecision(prec) << dur.count() / 1'000 << " microseconds" << '\n';
            }
        }
    };
}
