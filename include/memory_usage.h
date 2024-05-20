// memory_usage.h
#ifndef MEMORY_USAGE_H
#define MEMORY_USAGE_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

// Structure to store memory information
struct MemoryInfo {
    long totalMem = 0;    // Total system memory
    long freeMem = 0;     // Free memory that is not being used
    long availableMem = 0;  // Memory available for starting new applications, without swapping
};

class MemoryUsage {
public:
    // Function to read and parse memory information from /proc/meminfo
    static std::string get_memory_usage() {
        MemoryInfo memInfo;
        std::ifstream meminfo("/proc/meminfo");
        std::string line;

        while (std::getline(meminfo, line)) {
            std::istringstream iss(line);
            std::string key;
            long value;
            iss >> key >> value;  // Parse key and value from the line

            if (key == "MemTotal:") {
                memInfo.totalMem = value;
            } else if (key == "MemFree:") {
                memInfo.freeMem = value;
            } else if (key == "MemAvailable:") {
                memInfo.availableMem = value;
            }
        }

        std::ostringstream ss;
        ss << "Total Memory: " << memInfo.totalMem << " kB, "
           << "Free Memory: " << memInfo.freeMem << " kB, "
           << "Available Memory: " << memInfo.availableMem << " kB";
        return ss.str();
    }
};

#endif // MEMORY_USAGE_H
