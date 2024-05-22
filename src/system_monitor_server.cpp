#include <iostream>
#include <string>
#include <unistd.h>  // sleep(), close()
#include <sys/socket.h>
#include <netinet/in.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <cstring>   // for memset()
#include <iomanip>   // for std::setprecision
#include "cpu_usage.h"  // Include the header file for CPU usage functions
#include "disk_usage.h" // Include the header file for disk usage functions
#include "memory_usage.h"
#include "network_stats.h"

#define PORT 8080
const std::string API_KEY = "4ee511cfc743e7033b7451e090c6b00b"; // Your API key

void initializeSSL() {
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();
}

bool validate_api_key(const std::string& command, std::string& actual_command) {
    size_t separator = command.find(':');
    if (separator != std::string::npos) {
        std::string key = command.substr(0, separator);
        actual_command = command.substr(separator + 1);
        return key == API_KEY;
    }
    return false;
}

void process_command(const std::string& command, SSL* ssl) {
    std::string actual_command;
    if (!validate_api_key(command, actual_command)) {
        std::string response = "Invalid API key";
        SSL_write(ssl, response.c_str(), response.length());
        return;
    }

    std::string response;
    if (actual_command == "GET CPU") {
        CpuTime prev_cpu_time, curr_cpu_time;
        get_cpu_times(prev_cpu_time);
        sleep(1);
        get_cpu_times(curr_cpu_time);
        float cpu_usage = calculate_cpu_usage(prev_cpu_time, curr_cpu_time);
        std::ostringstream stream;
        stream << std::fixed << std::setprecision(2) << cpu_usage;
        response = "CPU Usage: " + stream.str() + "%";
    } else if (actual_command == "GET DISK") {
        response = DiskUsage::get_disk_usage("/");
    } else if (actual_command == "GET MEM") {
        response = MemoryUsage::get_memory_usage();
    } else if (actual_command == "GET NET") {
        response = NetworkStats::get_network_stats("wlan0");
    } else {
        response = "Unknown command";
    }
    SSL_write(ssl, response.c_str(), response.length());  // Send response back to client using SSL
}

int main() {
    initializeSSL();

    int server_fd, client_socket;
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);
    SSL_CTX* ctx = SSL_CTX_new(TLS_server_method());

    SSL_CTX_use_certificate_file(ctx, "server.crt", SSL_FILETYPE_PEM);
    SSL_CTX_use_PrivateKey_file(ctx, "server.key", SSL_FILETYPE_PEM);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    memset(&address, 0, sizeof(address));
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
            continue;
        }

        SSL* ssl = SSL_new(ctx);
        SSL_set_fd(ssl, client_socket);

        if (SSL_accept(ssl) <= 0) {
            ERR_print_errors_fp(stderr);
        } else {
            char buffer[1024] = {0};
            while (true) {
                memset(buffer, 0, sizeof(buffer));
                int valread = SSL_read(ssl, buffer, sizeof(buffer)-1);
                if (valread <= 0) {
                    int err = SSL_get_error(ssl, valread);
                    if (err == SSL_ERROR_ZERO_RETURN || err == SSL_ERROR_SYSCALL) {
                        std::cout << "Client disconnected.\n";
                        break;
                    }
                } else {
                    std::string command(buffer, valread);
                    process_command(command, ssl);
                }
            }
        }

        SSL_shutdown(ssl);
        SSL_free(ssl);
        close(client_socket);
    }

    SSL_CTX_free(ctx);
    close(server_fd);
    return 0;
}
