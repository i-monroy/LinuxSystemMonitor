#include <iostream>
#include <fstream>
#include <string>
#include <sstream>  // Include this for std::istringstream
#include <unistd.h>  // for sleep()
#include <iomanip>

struct CpuTime {
    long user = 0, nice = 0, system = 0, idle = 0, iowait = 0, irq = 0, softirq = 0, steal = 0, guest = 0, guest_nice = 0;
};

void get_cpu_times(CpuTime& times) {
    std::ifstream proc_stat("/proc/stat");
    std::string line;
    if (std::getline(proc_stat, line)) {  // Ensure the line is read successfully
        std::istringstream iss(line);
        std::string cpu;
        iss >> cpu >> times.user >> times.nice >> times.system >> times.idle >> times.iowait
            >> times.irq >> times.softirq >> times.steal >> times.guest >> times.guest_nice;
    }
}

double calculate_cpu_usage(const CpuTime& prev, const CpuTime& curr) {
    const long prev_idle = prev.idle + prev.iowait;
    const long curr_idle = curr.idle + curr.iowait;

    const long prev_non_idle = prev.user + prev.nice + prev.system + prev.irq + prev.softirq + prev.steal;
    const long curr_non_idle = curr.user + curr.nice + curr.system + curr.irq + curr.softirq + curr.steal;

    const long prev_total = prev_idle + prev_non_idle;
    const long curr_total = curr_idle + curr_non_idle;

    const long total_diff = curr_total - prev_total;
    const long idle_diff = curr_idle - prev_idle;

    return total_diff > 0 ? (total_diff - idle_diff) * 100.0 / total_diff : 0; // Avoid division by zero
}

int main() {
    CpuTime prev_cpu_time, curr_cpu_time;

    // Get initial CPU times
    get_cpu_times(prev_cpu_time);
    while (true) {
        sleep(1);  // Sleep for 1 second
        get_cpu_times(curr_cpu_time);

        // Calculate CPU usage since last check
        float cpu_usage = calculate_cpu_usage(prev_cpu_time, curr_cpu_time);
        std::cout << std::setprecision(2) << std::fixed;
        std::cout << "CPU Usage: " << cpu_usage << "%" << std::endl;

        // Update previous times
        prev_cpu_time = curr_cpu_time;
    }

    return 0;
}
