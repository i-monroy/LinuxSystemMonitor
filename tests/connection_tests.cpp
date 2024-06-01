#include <gtest/gtest.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> 

// Define a test fixture class for testing connection handling
class ConnectionHandlingTests : public ::testing::Test {
protected:
    int sock;  
    struct sockaddr_in server_addr;  

    // SetUp() is run before each test
    void SetUp() override {
        // Create a socket
        sock = socket(AF_INET, SOCK_STREAM, 0);
        // Assert that the socket creation did not fail
        ASSERT_NE(sock, -1) << "Failed to create socket.";

        // Configure server address
        server_addr.sin_family = AF_INET;  // Use IPv4 addresses
        server_addr.sin_port = htons(8000);  // Server listening port
        server_addr.sin_addr.s_addr = inet_addr("192.168.1.82");  // Server IP address

        // Set remaining bytes in the structure to zero
        memset(server_addr.sin_zero, '\0', sizeof server_addr.sin_zero);
    }

    // TearDown() is run after each test
    void TearDown() override {
        // Close the socket
        close(sock);
    }
};

// Test successful connection to the server
TEST_F(ConnectionHandlingTests, TestSuccessfulConnection) {
    // Attempt to connect to the server
    int result = connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    // Expect that the connection succeeds
    EXPECT_EQ(result, 0) << "Connection to the server failed.";
}

// Test failure to connect on a non-listening port
TEST_F(ConnectionHandlingTests, TestConnectionFailure) {
    // Change to a port likely not open for listening
    server_addr.sin_port = htons(12345);
    // Attempt to connect to the server
    int result = connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    // Expect that the connection fails
    EXPECT_NE(result, 0) << "Connection should fail but succeeded.";
}

// Test behavior when the client disconnects
TEST_F(ConnectionHandlingTests, TestClientDisconnection) {
    // Connect to the server
    int result = connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    // Check if connection was successful
    EXPECT_EQ(result, 0) << "Failed to connect to server.";

    // Simulate client disconnection by closing the socket
    close(sock);
}
