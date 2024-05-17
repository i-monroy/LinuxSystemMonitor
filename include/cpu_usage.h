// cpu_usage.h
#ifndef CPU_USAGE_H
#define CPU_USAGE_H

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <unistd.h>

struct CpuTime {
    long user = 0, nice = 0, system = 0, idle = 0, iowait = 0,
         irq = 0, softirq = 0, steal = 0, guest = 0, guest_nice = 0;
};

inline void get_cpu_times(CpuTime& times) {
    std::ifstream proc_stat("/proc/stat");
    std::string line;
    if (std::getline(proc_stat, line)) {
        std::istringstream iss(line);
        std::string cpu;
        iss >> cpu >> times.user >> times.nice >> times.system >> times.idle
            >> times.iowait >> times.irq >> times.softirq >> times.steal
            >> times.guest >> times.guest_nice;
    }
}

inline double calculate_cpu_usage(const CpuTime& prev, const CpuTime& curr) {
    const long prev_idle = prev.idle + prev.iowait;
    const long curr_idle = curr.idle + curr.iowait;

    const long prev_non_idle = prev.user + prev.nice + prev.system + prev.irq + prev.softirq + prev.steal;
    const long curr_non_idle = curr.user + curr.nice + curr.system + curr.irq + curr.softirq + curr.steal;

    const long prev_total = prev_idle + prev_non_idle;
    const long curr_total = curr_idle + curr_non_idle;

    const long total_diff = curr_total - prev_total;
    const long idle_diff = curr_idle - prev_idle;

    return total_diff > 0 ? (total_diff - idle_diff) * 100.0 / total_diff : 0;
}

#endif // CPU_USAGE_H
