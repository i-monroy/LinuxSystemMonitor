#include <iostream>      // Standard C++ library for input and output
#include <fstream>       // C++ header for file operations
#include <string>        // C++ header for string operations
#include <sstream>       // Include for std::istringstream to parse strings
#include <unistd.h>      // For sleep() function, to pause the program

// Structure to store memory information
struct MemoryInfo {
    long totalMem = 0;    // Total system memory
    long freeMem = 0;     // Free memory that is not being used
    long availableMem = 0;  // Memory available for starting new applications, without swapping
};

// Function to read and parse memory information from /proc/meminfo
void get_memory_info(MemoryInfo& memInfo) {
    std::ifstream meminfo("/proc/meminfo");  // Open the memory info system file
    std::string line;
    // Read lines from the file one by one
    while (std::getline(meminfo, line)) {
        std::istringstream iss(line);
        std::string key;
        long value;
        std::string unit;
        iss >> key >> value >> unit;  // Parse key, value, and unit from the line

        // Check the key and update the corresponding memory info field
        if (key == "MemTotal:") {
            memInfo.totalMem = value;  // Set total memory
        } else if (key == "MemFree:") {
            memInfo.freeMem = value;   // Set free memory
        } else if (key == "MemAvailable:") {
            memInfo.availableMem = value;  // Set available memory
        }
    }
}

// Function to display the memory usage
void display_memory_usage(const MemoryInfo& memInfo) {
    std::cout << "Total Memory: " << memInfo.totalMem << " kB" << std::endl;
    std::cout << "Free Memory: " << memInfo.freeMem << " kB" << std::endl;
    std::cout << "Available Memory: " << memInfo.availableMem << " kB" << std::endl;
}

// Main function to repeatedly check and display memory usage
int main() {
    MemoryInfo memoryInfo;

    while (true) {
        get_memory_info(memoryInfo);  // Update memory info
        display_memory_usage(memoryInfo);  // Display updated info
        sleep(5);  // Pause for 5 seconds before updating again
    }

    return 0;
}
