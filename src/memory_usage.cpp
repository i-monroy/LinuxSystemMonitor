#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <unistd.h>  // for sleep()

struct MemoryInfo {
    long totalMem = 0;
    long freeMem = 0;
    long availableMem = 0;
};

void get_memory_info(MemoryInfo& memInfo) {
    std::ifstream meminfo("/proc/meminfo");
    std::string line;
    while (std::getline(meminfo, line)) {
        std::istringstream iss(line);
        std::string key;
        long value;
        std::string unit;
        iss >> key >> value >> unit;

        if (key == "MemTotal:") {
            memInfo.totalMem = value;
        } else if (key == "MemFree:") {
            memInfo.freeMem = value;
        } else if (key == "MemAvailable:") {
            memInfo.availableMem = value;
        }
    }
}

void display_memory_usage(const MemoryInfo& memInfo) {
    std::cout << "Total Memory: " << memInfo.totalMem << " kB" << std::endl;
    std::cout << "Free Memory: " << memInfo.freeMem << " kB" << std::endl;
    std::cout << "Available Memory: " << memInfo.availableMem << " kB" << std::endl;
}

int main() {
    MemoryInfo memoryInfo;

    while (true) {
        get_memory_info(memoryInfo);
        display_memory_usage(memoryInfo);
        sleep(5);  // update every 5 seconds
    }

    return 0;
}