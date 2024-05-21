#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

#define PORT 8080
#define SERVER_IP "192.168.1.82"  // Replace with the actual server IP
#define MAX_RETRIES 5

void printHelp() {
    std::cout << "\nAvailable Commands:\n"
              << "  GET CPU        - Fetch the current CPU usage\n"
              << "  GET DISK       - Fetch the current Disk usage\n"
              << "  GET MEM        - Fetch the current Memory usage\n"
              << "  GET NET        - Fetch the current Network statistics\n"
              << "  HELP           - Display this help message\n"
              << "  EXIT           - Exit the client\n\n";
}

int connectToServer(SOCKET& sock, struct sockaddr_in& serv_addr) {
    int attempts = 0;
    while (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
        if (++attempts >= MAX_RETRIES) return -1; // Exceeded max retry attempts
        std::cout << "Connection failed, retrying..." << std::endl;
        Sleep(2000); // Wait for 2 seconds before trying again
    }
    return 0;
}

int main() {
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        std::cout << "WSAStartup failed: " << iResult << std::endl;
        return 1;
    }

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cout << "Socket creation error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    struct sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr);

    if (connectToServer(sock, serv_addr) < 0) {
        std::cout << "Unable to connect to server after several attempts." << std::endl;
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
            std::cout << "Exiting client." << std::endl;
            break;
        } else if (input == "HELP") {
            printHelp();
            continue;
        }

        send(sock, input.c_str(), input.length(), 0);
        std::cout << "Request sent: " << input << std::endl;

        int valread = recv(sock, buffer, sizeof(buffer), 0);
        if (valread > 0) {
            buffer[valread] = '\0';
            std::cout << "Server: " << buffer << std::endl;
        } else if (valread == 0) {
            std::cout << "Connection closed by server. Attempting to reconnect..." << std::endl;
            if (connectToServer(sock, serv_addr) < 0) {
                std::cout << "Reconnection failed. Exiting..." << std::endl;
                break;
            }
        } else {
            std::cout << "recv failed: " << WSAGetLastError() << std::endl;
        }
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}
