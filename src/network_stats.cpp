#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <unistd.h>  // for sleep()

struct NetworkInterface {
    std::string name;
    unsigned long long rxBytes; // Bytes received
    unsigned long long txBytes; // Bytes transmitted
};

void get_network_stats(NetworkInterface& iface) {
    std::ifstream file("/proc/net/dev");
    std::string line;
    while (getline(file, line)) {
        if (line.find(iface.name) != std::string::npos) {
            std::istringstream iss(line);
            iss >> iface.name; // Read interface name
            iss.ignore(256, ':'); // Skip until data section
            iss >> iface.rxBytes >> std::ws; // Read received bytes
            for (int i = 0; i < 8; i++) { // Skip other rx stats
                iss.ignore(256, ' ');
            }
            iss >> iface.txBytes; // Read transmitted bytes
            break;
        }
    }
}

void display_network_stats(const NetworkInterface& iface) {
    std::cout << "Interface: " << iface.name << std::endl;
    std::cout << "Received Bytes: " << iface.rxBytes << std::endl;
    std::cout << "Transmitted Bytes: " << iface.txBytes << std::endl;
}

int main() {
    NetworkInterface iface = {"eth0"}; // Replace "eth0" with your network interface name if different

    while (true) {
        get_network_stats(iface);
        display_network_stats(iface);
        sleep(5);  // Update every 5 seconds
    }

    return 0;
}