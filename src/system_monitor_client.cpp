#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <string>

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "libssl.lib")
#pragma comment(lib, "libcrypto.lib")

#define PORT 8080
#define SERVER_IP "192.168.1.82"  // Adjust as needed
#define API_KEY "4ee511cfc743e7033b7451e090c6b00b" // Your generated API key

void printHelp() {
    std::cout << "\nAvailable Commands:\n"
              << "  GET CPU        - Fetch the current CPU usage\n"
              << "  GET DISK       - Fetch the current Disk usage\n"
              << "  GET MEM        - Fetch the current Memory usage\n"
              << "  GET NET        - Fetch the current Network statistics\n"
              << "  HELP           - Display this help message\n"
              << "  EXIT           - Exit the client\n\n";
}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed.\n";
        return 1;
    }

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Socket creation error: " << WSAGetLastError() << '\n';
        WSACleanup();
        return 1;
    }

    sockaddr_in serv_addr {};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr);

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

    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, nullptr);
    if (!SSL_CTX_load_verify_locations(ctx, "C:\\Users\\monro\\MyApp\\certs\\ca.pem", nullptr)) {
        std::cerr << "Failed to load CA certificate.\n";
        SSL_CTX_free(ctx);
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    SSL* ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sock);

    if (connect(sock, (sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
        std::cerr << "Connection failed: " << WSAGetLastError() << '\n';
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    if (SSL_connect(ssl) != 1) {
        std::cerr << "SSL connection failed: " << SSL_get_error(ssl, 0) << '\n';
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    printHelp();
    std::string input;
    char buffer[1024] = {0};

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

        std::string full_command = std::string(API_KEY) + ":" + input; // Correctly form the full command
        SSL_write(ssl, full_command.c_str(), full_command.length());
        std::cout << "Request sent: " << input << '\n';

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

    SSL_shutdown(ssl);
    SSL_free(ssl);
    SSL_CTX_free(ctx);
    closesocket(sock);
    WSACleanup();
    return 0;
}
