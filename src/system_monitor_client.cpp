#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <string>

// Linking required libraries for networking and SSL functionalities
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "libssl.lib")
#pragma comment(lib, "libcrypto.lib")

#define PORT 8000  // Port number for the server connection
#define SERVER_IP "enter IP address"  // Server IP address
#define API_KEY "enter API key" // API key for authentication

// Function to display available commands to the user
void printHelp() {
    std::cout << "\nAvailable Commands:\n"
              << "  GET CPU        - Fetch the current CPU usage\n"
              << "  GET DISK       - Fetch the current Disk usage\n"
              << "  GET MEM        - Fetch the current Memory usage\n"
              << "  GET NET        - Fetch the current Network statistics\n"
              << "  HELP           - Display this help message\n"
              << "  EXIT           - Exit the client\n\n";
}

// Main function: Setup and manage the SSL client
int main(int argc, char* argv[]) {
    WSADATA wsaData;
    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed.\n";
        return 1;
    }

    // Create a socket
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Socket creation error: " << WSAGetLastError() << '\n';
        WSACleanup();
        return 1;
    }

    // Define server address
    sockaddr_in serv_addr {};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr);

    // Initialize SSL
    SSL_library_init();
    OpenSSL_add_ssl_algorithms();
    SSL_load_error_strings();
    SSL_CTX* ctx = SSL_CTX_new(TLS_client_method());
    if (!ctx) {
        std::cerr << "SSL_CTX creation failed.\n";
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // Load CA certificate
    if (!SSL_CTX_load_verify_locations(ctx, "C:\\Users\\monro\\MyApp\\certs\\ca.pem", nullptr)) {
        std::cerr << "Failed to load CA certificate.\n";
        SSL_CTX_free(ctx);
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // Create an SSL structure
    SSL* ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sock);

    // Connect to the server
    if (connect(sock, (sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
        std::cerr << "Connection failed: " << WSAGetLastError() << '\n';
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // Establish an SSL connection
    if (SSL_connect(ssl) != 1) {
        std::cerr << "SSL connection failed: " << SSL_get_error(ssl, 0) << '\n';
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // Display available commands
    printHelp();
    std::string input;
    char buffer[1024] = {0};

    // Main loop to process user commands
    while (true) {
        std::cout << "Enter command: ";
        std::getline(std::cin, input);
        if (input == "EXIT") {
            std::cout << "Exiting client.\n";
            break;
        } else if (input == "HELP") {
            printHelp();
            continue;
        }

        // Send command to server
        std::string full_command = API_KEY + ":" + input;  // Attach API key
        SSL_write(ssl, full_command.c_str(), full_command.length());
        std::cout << "Request sent: " << input << '\n';

        // Receive response from server
        int valread = SSL_read(ssl, buffer, sizeof(buffer)-1);
        if (valread > 0) {
            buffer[valread] = '\0';
            std::cout << "Server: " << buffer << '\n';
        } else {
            int err = SSL_get_error(ssl, valread);
            if (err == SSL_ERROR_ZERO_RETURN || err == SSL_ERROR_SYSCALL) {
                std::cout << "Connection closed by server.\n";
                break;
            } else {
                std::cout << "SSL read failed: " << ERR_error_string(err, nullptr) << '\n';
            }
        }
    }

    // Cleanup and exit
    SSL_shutdown(ssl);
    SSL_free(ssl);
    SSL_CTX_free(ctx);
    closesocket(sock);
    WSACleanup();
    return 0;
}
