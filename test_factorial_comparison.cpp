#include <iostream>
#include <chrono>
#include <unordered_map>
#include <string>
#include <iomanip>

class Timer {
private:
    struct TimerData {
        std::chrono::nanoseconds total_time{0};
        int call_count = 0;
    };
    
    std::unordered_map<std::string, TimerData> metrics;
    std::chrono::high_resolution_clock::time_point start_time;
    std::string current_timer;

public:
    void start(const std::string& name) {
        start_time = std::chrono::high_resolution_clock::now();
        current_timer = name;
    }

    void end(const std::string& name) {
        if (current_timer != name) {
            std::cerr << "Warning: Timer '" << name << "' not started properly" << std::endl;
            return;
        }

        auto end_time = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);

        metrics[name].total_time += elapsed;
        metrics[name].call_count++;
        current_timer.clear();
    }

    void display() {
        std::cout << "\n--- Performance Metrics ---" << std::endl;
        
        if (metrics.empty()) {
            std::cout << "  (No timing metrics collected)" << std::endl;
            return;
        }

        // Calculate max name length for formatting
        size_t max_name_len = 8; // minimum width
        for (const auto& pair : metrics) {
            max_name_len = std::max(max_name_len, pair.first.length());
        }

        std::cout << std::left << std::setw(max_name_len) << "Function"
                  << std::right << std::setw(8) << "Calls"
                  << std::setw(12) << "Total"
                  << std::setw(12) << "Average" << std::endl;
        
        std::cout << std::string(max_name_len + 32, '-') << std::endl;

        for (const auto& pair : metrics) {
            const std::string& name = pair.first;
            const TimerData& data = pair.second;
            
            long long total_ns = data.total_time.count();
            int calls = data.call_count;
            long long avg_ns = calls > 0 ? total_ns / calls : 0;

            auto format_time = [](long long nanoseconds) -> std::string {
                if (nanoseconds < 1000) {
                    return std::to_string(nanoseconds) + " ns";
                } else if (nanoseconds < 1000000) {
                    return std::to_string(nanoseconds / 1000.0) + " µs";
                } else if (nanoseconds < 1000000000) {
                    return std::to_string(nanoseconds / 1000000.0) + " ms";
                } else {
                    return std::to_string(nanoseconds / 1000000000.0) + " s";
                }
            };

            std::cout << std::left << std::setw(max_name_len) << name
                      << std::right << std::setw(8) << calls
                      << std::setw(12) << format_time(total_ns)
                      << std::setw(12) << format_time(avg_ns) << std::endl;
        }
    }

    long long get_total_ns(const std::string& name) {
        return metrics[name].total_time.count();
    }

    int get_call_count(const std::string& name) {
        return metrics[name].call_count;
    }

    void clear() {
        metrics.clear();
    }
};

// Global timer instance
Timer timer;

// Pure recursive factorial without internal timing
long long recursive_factorial(int n) {
    if (n <= 1) {
        return 1;
    } else {
        return n * recursive_factorial(n - 1);
    }
}

// Pure iterative factorial without internal timing
long long iter_factorial(int n) {
    long long result = 1;
    for (int i = 2; i <= n; ++i) {
        result *= i;
    }
    return result;
}

int main() {
    std::cout << "Hello" << std::endl;

    // Test both factorial implementations 10 times each
    for (int iteration = 0; iteration < 10; ++iteration) {
        // Time the entire recursive operation
        timer.start("recursive_factorial");
        long long recursive_result = recursive_factorial(18);
        timer.end("recursive_factorial");

        // Time the entire iterative operation
        timer.start("iter_factorial");
        long long iterative_result = iter_factorial(18);
        timer.end("iter_factorial");

        std::cout << "Calculating recursive Factorial of 18 = " << recursive_result 
                  << " AND = " << iterative_result << std::endl;
    }

    std::cout << "BYE" << std::endl;

    // Display timing metrics
    timer.display();

    // Example of querying specific metrics
    std::cout << "\nDetailed metrics:" << std::endl;
    std::cout << "Recursive calls: " << timer.get_call_count("recursive_factorial") << std::endl;
    std::cout << "Iterative calls: " << timer.get_call_count("iter_factorial") << std::endl;
    std::cout << "Recursive total time: " << timer.get_total_ns("recursive_factorial") << " ns" << std::endl;
    std::cout << "Iterative total time: " << timer.get_total_ns("iter_factorial") << " ns" << std::endl;

    return 0;
}