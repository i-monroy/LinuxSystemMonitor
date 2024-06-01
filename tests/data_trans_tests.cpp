#include <gtest/gtest.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <string>

// Define the API Key used for authentication in API requests
const std::string API_KEY = "your API key";

// Test class for SSL connection tests, inherits from testing::Test provided by Google Test
class SSLConnectionTests : public ::testing::Test {
protected:
    SSL_CTX* ctx;  // SSL context pointer, manages settings for SSL connections
    SSL* ssl;      // SSL connection state
    int sock;      // Socket file descriptor for the connection
    struct sockaddr_in server_addr;  // Server address data structure

    // Set up function executed before each test
    void SetUp() override {
        // Initialize SSL libraries
        SSL_library_init();
        OpenSSL_add_ssl_algorithms(); // Registers the available SSL/TLS ciphers and digests
        SSL_load_error_strings();     // Load SSL error strings for error reporting

        // Create new SSL context with client method, specifying the use of a client-side SSL/TLS method
        ctx = SSL_CTX_new(TLS_client_method());
        ASSERT_TRUE(ctx != nullptr) << "SSL_CTX creation failed.";

        // Set the verification mode to verify peer in SSL handshake
        SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, nullptr);
        // Load the CA certificate from specified path
        ASSERT_TRUE(SSL_CTX_load_verify_locations(ctx, "/home/pi/Desktop/LinuxSystemMonitor/ca.pem", nullptr)) << "Failed to load CA certificate.";

        // Create a socket with IPv4 address family, TCP stream type
        sock = socket(AF_INET, SOCK_STREAM, 0);
        ASSERT_NE(sock, -1) << "Failed to create socket.";

        // Configure server address struct
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(8000);  // Set server port to 8000
        server_addr.sin_addr.s_addr = inet_addr("192.168.1.82");  // Convert and set server IP address

        // Clear the rest of the struct
        memset(&server_addr.sin_zero, '\0', sizeof server_addr.sin_zero);
        
        // Initialize a new SSL structure for the connection
        ssl = SSL_new(ctx);
        // Set the file descriptor for the SSL connection
        SSL_set_fd(ssl, sock);
        // Attempt to connect to the specified socket address
        ASSERT_EQ(connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)), 0) << "Failed to connect.";
        // Attempt to initiate an SSL connection over the socket
        ASSERT_EQ(SSL_connect(ssl), 1) << "SSL connection failed.";
    }

    // Tear down function executed after each test
    void TearDown() override {
        // Clean up by shutting down the SSL connection, and freeing the memory for SSL and context
        SSL_shutdown(ssl);
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        // Close the socket
        close(sock);
    }
};

// Test to verify a valid SSL connection and valid command processing
TEST_F(SSLConnectionTests, TestValidSSLConnection) {
    // Formulate a valid request message with the API key
    std::string test_message = API_KEY + ":GET CPU";
    SSL_write(ssl, test_message.c_str(), test_message.length());

    // Buffer to read the response
    char buffer[1024] = {0};
    int bytes_read = SSL_read(ssl, buffer, sizeof(buffer)-1);
    ASSERT_GT(bytes_read, 0) << "Failed to read data.";
    std::string response(buffer);
    // Check if the response contains expected output
    EXPECT_NE(response.find("CPU Usage:"), std::string::npos) << "Unexpected response: " << response;
}

// Test sending malformed data but with a valid API key
TEST_F(SSLConnectionTests, TestMalformedData) {
    std::string malformed_message = API_KEY + ":MALFORMED DATA";
    SSL_write(ssl, malformed_message.c_str(), malformed_message.length());

    char buffer[1024] = {0};
    int bytes_read = SSL_read(ssl, buffer, sizeof(buffer)-1);
    ASSERT_GT(bytes_read, 0) << "Failed to read data.";
    std::string response(buffer);
    // Expect an error message indicating an unknown command
    EXPECT_NE(response.find("Unknown command"), std::string::npos) << "Unexpected response, should have indicated unknown or malformed command: " << response;
}

// Test sending an unsupported command to the server
TEST_F(SSLConnectionTests, TestUnsupportedCommand) {
    std::string unsupported_message = API_KEY + ":UNSUPPORTED_COMMAND";
    SSL_write(ssl, unsupported_message.c_str(), unsupported_message.length());

    char buffer[1024] = {0};
    int bytes_read = SSL_read(ssl, buffer, sizeof(buffer)-1);
    ASSERT_GT(bytes_read, 0) << "Failed to read data.";
    std::string response(buffer);
    // Expect an error message indicating an unknown command
    EXPECT_NE(response.find("Unknown command"), std::string::npos) << "Unexpected response, should have indicated unknown command: " << response;
}
