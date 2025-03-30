//
// A general purpose performance timer in a header-only library with delayed
// logging.
//

#pragma once

#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

namespace perftimer {
    // Version of the performance timer library
    constexpr auto VERSION = "0.7.2";

    // Type alias for high-resolution clock and time point
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = std::chrono::time_point<Clock>;

    // Performance Timer class to measure execution time
    struct PerfTimer {
      private:
        // Constants for time conversions
        constexpr static auto billions = 1'000'000'000;  // 1 billion nanoseconds
        constexpr static auto millions = 1'000'000;      // 1 million nanoseconds

        TimePoint t0;  // Start time point
        TimePoint t1;  // End time point

        std::string name;  // Name of the timer for identification

      public:
        // Constructor that initializes the timer with a unique name
        explicit PerfTimer(std::string timer_name) : name(std::move(timer_name)) {};

        // Output stream for buffered logging; allows delayed console output
        std::ostringstream log;

        // Precision for displaying time; defaults to 9 decimal places
        int prec = 9;

        // Start or restart the timer; call stop() to capture the duration
        void start() {
            t0 = Clock::now();  // Record the current time as start time
            t1 = Clock::now();  // set the stop time to ensure it's initialized
        }

        // Stop the timer and record the end time
        void stop() {
            t1 = Clock::now();  // Record the current time as end time
        }

        // Calculate and return the duration in nanoseconds between start and end
        auto get_duration() {
            const std::chrono::duration<double, std::nano> dur = t1 - t0;  // Calculate duration
            return dur;                                                    // Return duration
        }

        // create a duration message and return the string
        std::string get_duration_string(const std::string &message = ": process took: ") {
            auto dur = get_duration();  // Get the duration
            std::ostringstream buffer;

            // Output the duration in appropriate units
            if (dur.count() > billions) {
                buffer << name << message << std::setprecision(prec) << dur.count() / billions << " seconds";
            } else if (dur.count() > millions) {
                buffer << name << message << std::setprecision(prec) << dur.count() / millions << " milliseconds";
            } else {
                buffer << name << message << std::setprecision(prec) << dur.count() / 1'000 << " microseconds";
            }

            return buffer.str();
        }

        // Display the duration with a custom message
        void show_duration(const std::string &message = ": process took: ") {
            std::cout << get_duration_string(message) << '\n';
        }
    };
}  // namespace perftimer
