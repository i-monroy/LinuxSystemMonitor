#include <iostream>    // Include for standard I/O operations
#include <fstream>     // Include for file stream operations
#include <string>      // Include for using the std::string class
#include <sstream>     // Include for string stream operations
#include <vector>      // Include for using the std::vector container
#include <unistd.h>    // Include for POSIX operating system API, specifically sleep()

// Define a structure to hold network interface statistics
struct NetworkInterface {
    std::string name;
    unsigned long long rxBytes; // Bytes received
    unsigned long long txBytes; // Bytes transmitted
};

// Function to extract network stats from /proc/net/dev
void get_network_stats(NetworkInterface& iface) {
    std::ifstream file("/proc/net/dev"); // Open the file containing network device status
    std::string line;
    while (getline(file, line)) { // Read each line from the file
        if (line.find(iface.name) != std::string::npos) { // Check if the line contains the interface name
            std::istringstream iss(line); // Use a string stream to parse the line
            std::vector<std::string> tokens; // Vector to store each element from the line
            std::string token;
            while (iss >> token) { // Extract each space-separated element (token) from the line
                tokens.push_back(token);
            }
            // Assuming tokens[1] is RX bytes and tokens[9] is TX bytes after the interface name
            // Make sure the vector has at least 10 elements to avoid accessing out-of-bounds
            if (tokens.size() > 9) {  
                iface.rxBytes = std::stoull(tokens[1]); // Convert string to unsigned long long
                iface.txBytes = std::stoull(tokens[9]);
            }
            break;
        }
    }
}

// Function to display the network stats
void display_network_stats(const NetworkInterface& iface) {
    std::cout << "Interface: " << iface.name << std::endl;
    std::cout << "Received Bytes: " << iface.rxBytes << std::endl;
    std::cout << "Transmitted Bytes: " << iface.txBytes << std::endl;
}

// Main function to repeatedly check and display network stats
int main() {
    NetworkInterface iface = {"wlan0"}; // Specify the network interface to monitor

    while (true) {
        get_network_stats(iface); // Get current stats
        display_network_stats(iface); // Display stats
        sleep(5);  // Pause for 5 seconds before updating
    }

    return 0;
}