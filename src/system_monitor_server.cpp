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
#include <iostream>
#include <string>
#include <nlohmann/json.hpp>
#include <fstream>
#include <curl/curl.h>
#include <chrono>
#include <openssl/sha.h>
#include <cstdlib>
#include <fstream>
#include <cctype>
#include <thread>
#include <vector>
#include <algorithm> // for std::all_of


#define PORT 8000
const std::string API_KEY = "4ee511cfc743e7033b7451e090c6b00b"; // Your API key

using json = nlohmann::json;

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

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *s) {
    size_t newLength = size * nmemb;
    try {
        s->append((char*)contents, newLength);
        return newLength;
    }
    catch (std::bad_alloc &e) {
        // handle memory problem
        return 0;
    }
}

// Helper function to download file
static size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

void download_update(const std::string& url, const std::string& output_path) {
    CURL *curl;
    FILE *fp;
    CURLcode res;

    curl = curl_easy_init();
    if (curl) {
        fp = fopen(output_path.c_str(),"wb");
        if (!fp) {
            std::cerr << "Failed to open file for writing: " << output_path << std::endl;
            curl_easy_cleanup(curl);
            return;  // Exit if file cannot be opened
        }

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        res = curl_easy_perform(curl);
        fclose(fp);  // Close the file pointer here to avoid leaks

        if(res != CURLE_OK) {
            std::cerr << "Failed to download update: " << curl_easy_strerror(res) << std::endl;
        } else {
            std::cout << "Update downloaded successfully." << std::endl;
        }
        curl_easy_cleanup(curl);
    }
}


std::string calculate_sha256(const std::string& file_path) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);

    std::ifstream file(file_path, std::ifstream::binary);
    if (!file) {
        std::cerr << "Cannot open file: " << file_path << std::endl;
        return "";
    }

    const int bufSize = 32768;
    char* buffer = new char[bufSize];
    while (file.good()) {
        file.read(buffer, bufSize);
        SHA256_Update(&sha256, buffer, file.gcount());
    }

    SHA256_Final(hash, &sha256);
    file.close();
    delete[] buffer;

    std::stringstream ss;
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }

    return ss.str();
}

void apply_update(const std::string& path) {
    std::cout << "Applying update from: " << path << std::endl;
    // Example command to replace an executable file (adjust as needed)
    std::string command = "cp " + path + " ./build/system_monitor_server";
    std::system(command.c_str());
    std::cout << "Update applied. Restart the application to use the updated version." << std::endl;
}

void log_message(const std::string& message) {
    std::ofstream logFile("update_log.txt", std::ios::app); // Open in append mode
    logFile << message << std::endl;
}

void check_version(); // Forward declaration

// Function to periodically check for updates
void periodic_version_check(int intervalMinutes) {
    while (true) {
        check_version();
        std::this_thread::sleep_for(std::chrono::minutes(intervalMinutes));
    }
}

void check_version() {
    log_message("Checking for updates...");
    CURL *curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:5000/version-check");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if(res == CURLE_OK) {
            log_message("Update check successful.");
            json response = json::parse(readBuffer);
            std::string latest_version = response["version"];
            std::string update_url = response["update_url"];
            std::string expected_checksum = response["checksum"]; // Make sure this key exists in your Python server response
            
            std::string current_version = "1.0.0"; // Assume current version is defined elsewhere or hardcoded
            if(latest_version != current_version) {
                std::cout << "New version available: " << latest_version << std::endl;
                std::string download_path = "./update.zip";
                download_update(update_url, download_path);

                std::string actual_checksum = calculate_sha256(download_path);
                if(actual_checksum == expected_checksum) {
                    std::cout << "Checksum verification successful. Update is valid." << std::endl;
                    apply_update(download_path); // Apply the update
                } else {
                    std::cerr << "Checksum verification failed. Update is not valid." << std::endl;
                }
            } else {
                std::cout << "No updates available." << std::endl;
            }
        } else {
            log_message("Failed to check for updates: " + std::string(curl_easy_strerror(res)));
            std::cerr << "Failed to check for updates: " << curl_easy_strerror(res) << std::endl;
        }
    }
}

void process_command(const std::string& command, SSL* ssl) {
    std::string actual_command;
    if (!validate_api_key(command, actual_command)) {
        std::string response = "HTTP/1.1 403 Forbidden\r\n"
                               "Content-Type: text/plain\r\n"
                               "Content-Length: 17\r\n\r\n"
                               "Invalid API key";
        SSL_write(ssl, response.c_str(), response.length());
        return;
    }

    std::ostringstream response;
    response << "HTTP/1.1 200 OK\r\n"
             << "Content-Type: text/plain\r\n";

    if (actual_command == "GET CPU") {
        CpuTime prev_cpu_time, curr_cpu_time;
        get_cpu_times(prev_cpu_time);
        sleep(1);  // Simulate processing time
        get_cpu_times(curr_cpu_time);
        float cpu_usage = calculate_cpu_usage(prev_cpu_time, curr_cpu_time);
        response << "Content-Length: " << std::to_string(std::to_string(cpu_usage).length() + 14) << "\r\n\r\n"
                 << "CPU Usage: " << std::fixed << std::setprecision(2) << cpu_usage << "%";
    } else if (actual_command == "GET DISK") {
        std::string disk_usage = DiskUsage::get_disk_usage("/");
        response << "Content-Length: " << disk_usage.length() << "\r\n\r\n"
                 << disk_usage;
    } else if (actual_command == "GET MEM") {
        std::string memory_usage = MemoryUsage::get_memory_usage();
        response << "Content-Length: " << memory_usage.length() << "\r\n\r\n"
                 << memory_usage;
    } else if (actual_command == "GET NET") {
        std::string network_stats = NetworkStats::get_network_stats("wlan0");
        response << "Content-Length: " << network_stats.length() << "\r\n\r\n"
                 << network_stats;
    } else {
        response << "Content-Length: 15\r\n\r\n"
                 << "Unknown command";
    }

    std::string final_response = response.str();
    SSL_write(ssl, final_response.c_str(), final_response.length());  // Send response back to client using SSL
}

void handle_client(int client_socket, SSL_CTX* ctx) {
    SSL* ssl = SSL_new(ctx);
    SSL_set_fd(ssl, client_socket);

    if (SSL_accept(ssl) > 0) {
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
                process_command(command, ssl);  // Call process_command without capturing return value
            }
        }
    } else {
        ERR_print_errors_fp(stderr);
    }

    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(client_socket);
}

int main(int argc, char* argv[]) {
    initializeSSL();

    int server_fd;
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

    if (listen(server_fd, 10) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // Start the OTA update checking thread
    int checkInterval = (argc > 1 && std::all_of(argv[1], argv[1] + strlen(argv[1]), ::isdigit))
                        ? std::stoi(argv[1])
                        : 60; // Default to 60 minutes if no valid argument provided
    std::thread version_thread([checkInterval]() { periodic_version_check(checkInterval); });

    std::vector<std::thread> threads;

    while (true) {
        int client_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);
        if (client_socket < 0) {
            perror("accept");
            continue;
        }

        // Spawn a new thread for each accepted client connection
        threads.emplace_back([&](int socket) {
            handle_client(socket, ctx);
        }, client_socket);
    }

    // Join all threads (this will effectively never happen in this setup since the server runs indefinitely)
    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    // Clean up and shut down
    version_thread.join(); // Ensure the version checking thread is also cleanly exited before closing
    SSL_CTX_free(ctx);
    close(server_fd);
    return 0;
}