#include <gtest/gtest.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <string>

// Define the API Key
const std::string API_KEY = "4ee511cfc743e7033b7451e090c6b00b"; // Your API key

class SSLConnectionTests : public ::testing::Test {
protected:
    SSL_CTX* ctx;
    SSL* ssl;
    int sock;
    struct sockaddr_in server_addr;

    void SetUp() override {
        // SSL library initialization
        SSL_library_init();
        OpenSSL_add_ssl_algorithms();
        SSL_load_error_strings();
        
        ctx = SSL_CTX_new(TLS_client_method());
        ASSERT_TRUE(ctx != nullptr) << "SSL_CTX creation failed.";

        SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, nullptr);
        ASSERT_TRUE(SSL_CTX_load_verify_locations(ctx, "/home/pi/Desktop/LinuxSystemMonitor/ca.pem", nullptr)) << "Failed to load CA certificate.";

        // Socket setup
        sock = socket(AF_INET, SOCK_STREAM, 0);
        ASSERT_NE(sock, -1) << "Failed to create socket.";

        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(8000);  // Server port
        server_addr.sin_addr.s_addr = inet_addr("192.168.1.82");  // Server IP

        memset(&server_addr.sin_zero, '\0', sizeof server_addr.sin_zero);
        
        // SSL setup
        ssl = SSL_new(ctx);
        SSL_set_fd(ssl, sock);
        ASSERT_EQ(connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)), 0) << "Failed to connect.";
        ASSERT_EQ(SSL_connect(ssl), 1) << "SSL connection failed.";
    }

    void TearDown() override {
        SSL_shutdown(ssl);
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        close(sock);
    }
};

TEST_F(SSLConnectionTests, TestValidSSLConnection) {
    // Send and receive test
    std::string test_message = API_KEY + ":GET CPU";  // Correctly form the full command with API key
    SSL_write(ssl, test_message.c_str(), test_message.length());

    char buffer[1024] = {0};
    int bytes_read = SSL_read(ssl, buffer, sizeof(buffer)-1);
    ASSERT_GT(bytes_read, 0) << "Failed to read data.";
    std::string response(buffer);
    EXPECT_NE(response.find("CPU Usage:"), std::string::npos) << "Unexpected response: " << response;
}

TEST_F(SSLConnectionTests, TestMalformedData) {
    // Sending malformed data that still contains a valid API key
    std::string malformed_message = API_KEY + ":MALFORMED DATA";
    SSL_write(ssl, malformed_message.c_str(), malformed_message.length());

    char buffer[1024] = {0};
    int bytes_read = SSL_read(ssl, buffer, sizeof(buffer)-1);
    ASSERT_GT(bytes_read, 0) << "Failed to read data.";
    std::string response(buffer);
    // Update this check to expect the current behavior, adjust as needed if server logic is updated.
    EXPECT_NE(response.find("Unknown command"), std::string::npos) << "Unexpected response, should have indicated unknown or malformed command: " << response;
}

TEST_F(SSLConnectionTests, TestUnsupportedCommand) {
    // Sending an unsupported command
    std::string unsupported_message = API_KEY + ":UNSUPPORTED_COMMAND";
    SSL_write(ssl, unsupported_message.c_str(), unsupported_message.length());

    char buffer[1024] = {0};
    int bytes_read = SSL_read(ssl, buffer, sizeof(buffer)-1);
    ASSERT_GT(bytes_read, 0) << "Failed to read data.";
    std::string response(buffer);
    EXPECT_NE(response.find("Unknown command"), std::string::npos) << "Unexpected response, should have indicated unknown command: " << response;
}