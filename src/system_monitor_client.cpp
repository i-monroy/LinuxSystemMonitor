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
    std::string request = "GET CPU";  // Requesting CPU usage

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cout << "Socket creation error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 addresses from text to binary form
    iResult = inet_pton(AF_INET, "192.168.1.67", &serv_addr.sin_addr);
    if (iResult <= 0) {
        std::cout << "Invalid address/ Address not supported" << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // Connect to server
    iResult = connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (iResult == SOCKET_ERROR) {
        std::cout << "Connection failed: " << WSAGetLastError() << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // Send request to server
    send(sock, request.c_str(), request.length(), 0);
    std::cout << "Request sent: " << request << std::endl;

    // Read response from server
    int valread = recv(sock, buffer, 1024, 0);
    if (valread > 0) {
        std::cout << "Server: " << buffer << std::endl;
    } else if (valread == 0) {
        std::cout << "Connection closed" << std::endl;
    } else {
        std::cout << "recv failed: " << WSAGetLastError() << std::endl;
    }

    // Cleanup
    closesocket(sock);
    WSACleanup();
    return 0;
}