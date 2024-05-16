#include <iostream>    // Standard C++ library for input and output
#include <fstream>     // C++ header for file operations
#include <string>      // C++ header for string operations
#include <sstream>     // Include for std::istringstream to parse strings
#include <unistd.h>    // For sleep() function
#include <iomanip>     // For std::setprecision to format output

// Structure to store CPU times for different states
struct CpuTime {
    long user = 0, nice = 0, system = 0, idle = 0, iowait = 0, 
         irq = 0, softirq = 0, steal = 0, guest = 0, guest_nice = 0;
};

// Function to read CPU times from /proc/stat
void get_cpu_times(CpuTime& times) {
    std::ifstream proc_stat("/proc/stat");  // Open the file /proc/stat which contains the CPU times
    std::string line;
    // Read the first line which contains the times
    if (std::getline(proc_stat, line)) {  
        std::istringstream iss(line);      // Use istringstream for parsing the line
        std::string cpu;
        // Extract values into times structure
        iss >> cpu >> times.user >> times.nice >> times.system >> times.idle 
            >> times.iowait >> times.irq >> times.softirq >> times.steal 
            >> times.guest >> times.guest_nice;
    }
}

// Function to calculate CPU usage from previous and current CPU times
double calculate_cpu_usage(const CpuTime& prev, const CpuTime& curr) {
    const long prev_idle = prev.idle + prev.iowait; // Idle times in the previous interval
    const long curr_idle = curr.idle + curr.iowait; // Idle times in the current interval

    const long prev_non_idle = prev.user + prev.nice + prev.system + prev.irq + prev.softirq + prev.steal;
    const long curr_non_idle = curr.user + curr.nice + curr.system + curr.irq + curr.softirq + curr.steal;

    const long prev_total = prev_idle + prev_non_idle; // Total time previously
    const long curr_total = curr_idle + curr_non_idle; // Total time currently

    const long total_diff = curr_total - prev_total;  // Total time difference between intervals
    const long idle_diff = curr_idle - prev_idle;     // Idle time difference between intervals

    // Calculate the percentage of CPU usage
    return total_diff > 0 ? (total_diff - idle_diff) * 100.0 / total_diff : 0; // Avoid division by zero
}

// Main function to repeatedly check CPU usage
int main() {
    CpuTime prev_cpu_time, curr_cpu_time;

    // Get initial CPU times
    get_cpu_times(prev_cpu_time);
    while (true) {
        sleep(1);  // Sleep for 1 second between checks
        get_cpu_times(curr_cpu_time); // Update current CPU times

        // Calculate and display CPU usage since last check
        float cpu_usage = calculate_cpu_usage(prev_cpu_time, curr_cpu_time);
        std::cout << std::setprecision(2) << std::fixed;
        std::cout << "CPU Usage: " << cpu_usage << "%" << std::endl;

        // Update previous times for next calculation
        prev_cpu_time = curr_cpu_time;
    }

    return 0;
}
