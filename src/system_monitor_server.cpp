#include <iostream>
#include <string>
#include <unistd.h>  // sleep(), close()
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>   // for memset()
#include <iomanip>   // for std::setprecision
#include "cpu_usage.h"  // Include the header file for CPU usage functions
#include "disk_usage.h" // Include the header file for disk usage functions

#define PORT 8080

void process_command(const std::string& command, int client_socket) {
    std::string response;
    if (command == "GET CPU") {
        CpuTime prev_cpu_time, curr_cpu_time;  // Define CPU time structures
        get_cpu_times(prev_cpu_time);  // Get initial CPU times
        sleep(1);  // Wait a bit to get a new sample
        get_cpu_times(curr_cpu_time);  // Get updated CPU times
        float cpu_usage = calculate_cpu_usage(prev_cpu_time, curr_cpu_time);  // Calculate CPU usage
        std::ostringstream stream;
        stream << std::fixed << std::setprecision(2) << cpu_usage;  // Format to two decimal places
        response = "CPU Usage: " + stream.str() + "%";  // Create response string with formatted CPU usage
    } else if (command == "GET DISK") {
        response = DiskUsage::get_disk_usage("/");  // Fetch disk usage using the static function from DiskUsage class
    } else {
        response = "Unknown command";
    }
    send(client_socket, response.c_str(), response.length(), 0);  // Send response back to client
    std::cout << "Response sent: " << response << std::endl; // Debug print
}

int main() {
    int server_fd, client_socket;
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);
    char buffer[1024] = {0};

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    memset(&address, 0, sizeof(address));  // Clear structure
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    while (true) {
        if ((client_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen)) < 0) {
            perror("accept");
            continue;  // Continue to accept next connections
        }

        // Keep connection open to handle multiple requests
        while (true) {
            memset(buffer, 0, sizeof(buffer));  // Clear buffer
            long valread = read(client_socket, buffer, 1023);  // Read data sent by the client, ensure buffer is null-terminated
            if (valread < 0) {
                std::cerr << "Failed to read from socket." << std::endl;
                break;  // Break the loop and close the socket on error
            }
            if (valread == 0) {
                std::cout << "Client disconnected." << std::endl;
                break;  // Break the loop if client disconnects cleanly
            }

            std::string command(buffer, valread);  // Convert to string for easier processing
            process_command(command, client_socket);  // Process received command
        }

        close(client_socket);  // Close the connection
    }

    close(server_fd);  // Close the server socket
    return 0;
}
