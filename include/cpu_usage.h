#ifndef CPU_USAGE_H
#define CPU_USAGE_H

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <unistd.h>

// Structure to store CPU times for different CPU activities.
struct CpuTime {
    long user = 0, nice = 0, system = 0, idle = 0, iowait = 0,
         irq = 0, softirq = 0, steal = 0, guest = 0, guest_nice = 0;
};

// Reads CPU times from the /proc/stat file. It parses the first line which includes total CPU times.
inline void get_cpu_times(CpuTime& times) {
    std::ifstream proc_stat("/proc/stat");  // Open /proc/stat which contains the CPU times.
    std::string line;
    // Read the first line which contains the total CPU times.
    if (std::getline(proc_stat, line)) {
        std::istringstream iss(line);  // Use istringstream to parse the line.
        std::string cpu;  // Temporary to discard the 'cpu' prefix in the stat file.
        iss >> cpu >> times.user >> times.nice >> times.system >> times.idle
            >> times.iowait >> times.irq >> times.softirq >> times.steal
            >> times.guest >> times.guest_nice;
    }
}

// Calculates the CPU usage based on previous and current CPU times.
// It returns the CPU usage percentage.
inline double calculate_cpu_usage(const CpuTime& prev, const CpuTime& curr) {
    const long prev_idle = prev.idle + prev.iowait;
    const long curr_idle = curr.idle + curr.iowait;

    const long prev_non_idle = prev.user + prev.nice + prev.system + prev.irq + prev.softirq + prev.steal;
    const long curr_non_idle = curr.user + curr.nice + curr.system + curr.irq + curr.softirq + curr.steal;

    const long prev_total = prev_idle + prev_non_idle;
    const long curr_total = curr_idle + curr_non_idle;

    const long total_diff = curr_total - prev_total;
    const long idle_diff = curr_idle - prev_idle;

    // Calculate CPU usage as the percentage of time spent not idle in the total time.
    return total_diff > 0 ? (total_diff - idle_diff) * 100.0 / total_diff : 0;
}

#endif // CPU_USAGE_H
