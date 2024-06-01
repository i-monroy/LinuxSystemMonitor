#ifndef NETWORK_STATS_H
#define NETWORK_STATS_H

#include <iostream>   
#include <fstream>   
#include <sstream>    
#include <string>   
#include <vector>   
#include <netinet/in.h> 

// Structure to hold statistics for a network interface
struct NetworkInterface {
    std::string name;            
    unsigned long long rxBytes;   
    unsigned long long txBytes;   
};

class NetworkStats {
public:
    // Retrieves network statistics for a specified interface name from the /proc/net/dev file.
    // It formats these statistics into a string that includes received and transmitted bytes.
    static std::string get_network_stats(const std::string& iface_name) {
        NetworkInterface iface = {iface_name, 0, 0}; // Initialize with the interface name and zeroed statistics
        std::ifstream file("/proc/net/dev"); 
        std::string line; 

        while (getline(file, line)) {
            if (line.find(iface.name) != std::string::npos) { // Check if the line contains the interface name
                std::istringstream iss(line); // Use a string stream to parse the line
                std::vector<std::string> tokens; // Vector to store each element from the line
                std::string token;

                // Extract each space-separated element (token) from the line
                while (iss >> token) {
                    tokens.push_back(token);
                }

                // Assuming tokens[1] is RX bytes and tokens[9] is TX bytes after the interface name
                // Make sure the vector has at least 10 elements to avoid accessing out-of-bounds
                if (tokens.size() > 9) {
                    iface.rxBytes = std::stoull(tokens[1]);
                    iface.txBytes = std::stoull(tokens[9]);
                }
                break; // Exit the loop after processing the correct interface
            }
        }

        // Use an ostringstream to format the output string
        std::ostringstream ss;
        ss << "Interface: " << iface.name
           << ", Received Bytes: " << iface.rxBytes
           << ", Transmitted Bytes: " << iface.txBytes;
        return ss.str(); // Return the formatted statistics as a string
    }
};

#endif // NETWORK_STATS_H
