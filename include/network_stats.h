#ifndef NETWORK_STATS_H
#define NETWORK_STATS_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <netinet/in.h>

// Define a structure to hold network interface statistics
struct NetworkInterface {
    std::string name;
    unsigned long long rxBytes; // Bytes received
    unsigned long long txBytes; // Bytes transmitted
};

class NetworkStats {
public:
    // Function to extract network stats from /proc/net/dev
    static std::string get_network_stats(const std::string& iface_name) {
        NetworkInterface iface = {iface_name, 0, 0}; // Initialize with the interface name
        std::ifstream file("/proc/net/dev");
        std::string line;
        while (getline(file, line)) {
            if (line.find(iface.name) != std::string::npos) {
                std::istringstream iss(line);
                std::vector<std::string> tokens;
                std::string token;
                while (iss >> token) {
                    tokens.push_back(token);
                }
                if (tokens.size() > 9) {
                    iface.rxBytes = std::stoull(tokens[1]);
                    iface.txBytes = std::stoull(tokens[9]);
                }
                break;
            }
        }
        std::ostringstream ss;
        ss << "Interface: " << iface.name
           << ", Received Bytes: " << iface.rxBytes
           << ", Transmitted Bytes: " << iface.txBytes;
        return ss.str();
    }
};

#endif // NETWORK_STATS_H
