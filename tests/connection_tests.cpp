#include <gtest/gtest.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>  // for close()

class ConnectionHandlingTests : public ::testing::Test {
protected:
    int sock;
    struct sockaddr_in server_addr;

    void SetUp() override {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        ASSERT_NE(sock, -1) << "Failed to create socket.";

        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(8000);  // Server listening port
        server_addr.sin_addr.s_addr = inet_addr("192.168.1.82");  // Server IP address

        memset(server_addr.sin_zero, '\0', sizeof server_addr.sin_zero);
    }

    void TearDown() override {
        close(sock);
    }
};

TEST_F(ConnectionHandlingTests, TestSuccessfulConnection) {
    int result = connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    EXPECT_EQ(result, 0) << "Connection to the server failed.";
}

TEST_F(ConnectionHandlingTests, TestConnectionFailure) {
    server_addr.sin_port = htons(12345);  // Use a port likely not to be listening
    int result = connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    EXPECT_NE(result, 0) << "Connection should fail but succeeded.";
}

TEST_F(ConnectionHandlingTests, TestClientDisconnection) {
    int result = connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    EXPECT_EQ(result, 0) << "Failed to connect to server.";

    // Simulate client disconnection
    close(sock);

    // You could reconnect to check how server logs or handles the disconnection
    // This might involve more complex server behavior checking or logging
}

// Further tests can be added here
