#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

#define PORT 8080

int main() {
    WSADATA wsaData;
    int iResult;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        std::cout << "WSAStartup failed: " << iResult << std::endl;
        return 1;
    }

    SOCKET sock = INVALID_SOCKET;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cout << "Socket creation error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "192.168.1.67", &serv_addr.sin_addr);

    // Connect to server
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
        std::cout << "Connection failed: " << WSAGetLastError() << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // Multiple requests in the same session
    std::string requests[] = {"GET CPU", "GET DISK"};
    for (const std::string& request : requests) {
        send(sock, request.c_str(), request.length(), 0);
        std::cout << "Request sent: " << request << std::endl;

        int valread = recv(sock, buffer, sizeof(buffer), 0); // Adjust size to sizeof(buffer) for safety
        if (valread > 0) {
            buffer[valread] = '\0'; // Ensure null termination
            std::cout << "Server: " << buffer << std::endl;
        } else if (valread == 0) {
            std::cout << "Connection closed by server" << std::endl;
            break; // Break the loop if server closes the connection
        } else {
            std::cout << "recv failed: " << WSAGetLastError() << std::endl;
        }
    }

    // Cleanup
    closesocket(sock);
    WSACleanup();
    return 0;
}
