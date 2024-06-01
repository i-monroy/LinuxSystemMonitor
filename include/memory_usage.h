#ifndef MEMORY_USAGE_H
#define MEMORY_USAGE_H

#include <iostream>   
#include <fstream>   
#include <sstream>    
#include <string>  

// Structure to store memory information.
struct MemoryInfo {
    long totalMem = 0;       // Total system memory 
    long freeMem = 0;        // Free memory a
    long availableMem = 0;   // Memory available for starting new applications
};

// Class to encapsulate memory usage operations
class MemoryUsage {
public:
    // Retrieves memory usage statistics from the system and returns them as a formatted string.
    // This function reads from the /proc/meminfo file, which contains the memory statistics for the system.
    static std::string get_memory_usage() {
        MemoryInfo memInfo;  // MemoryInfo structure to hold memory values
        std::ifstream meminfo("/proc/meminfo");  // Open the memory info system file
        std::string line;  

        // Read lines from the file one by one
        while (std::getline(meminfo, line)) {
            std::istringstream iss(line);  
            std::string key;  
            long value;  
            iss >> key >> value;  // Extract key and value from the line

            // Update the corresponding field in memInfo based on the key
            if (key == "MemTotal:") {
                memInfo.totalMem = value;  
            } else if (key == "MemFree:") {
                memInfo.freeMem = value;  
            } else if (key == "MemAvailable:") {
                memInfo.availableMem = value;  
            }
        }

        // Use an ostringstream to format the output string
        std::ostringstream ss;
        ss << "Total Memory: " << memInfo.totalMem << " kB, "
           << "Free Memory: " << memInfo.freeMem << " kB, "
           << "Available Memory: " << memInfo.availableMem << " kB";
        return ss.str();  // Return the formatted string
    }
};

#endif // MEMORY_USAGE_H
