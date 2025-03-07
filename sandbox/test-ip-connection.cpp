#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <chrono>
#include <thread>

bool test_socket_connection(const std::string& host, int port, int timeout_ms) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Socket creation failed" << std::endl;
        return false;
    }

    struct sockaddr_in server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    
    // Convert hostname to IP address
    if (inet_pton(AF_INET, host.c_str(), &server_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address or address not supported" << std::endl;
        close(sock);
        return false;
    }

    // Set the socket to non-blocking
    fcntl(sock, F_SETFL, O_NONBLOCK);

    // Attempt to connect
    int result = connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (result < 0) {
        if (errno != EINPROGRESS) {
            std::cerr << "Connection failed" << std::endl;
            close(sock);
            return false;
        }
    }

    // Wait for the connection to complete
    fd_set fdset;
    FD_ZERO(&fdset);
    FD_SET(sock, &fdset);
    struct timeval tv;
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;

    result = select(sock + 1, nullptr, &fdset, nullptr, &tv);
    if (result > 0) {
        int so_error;
        socklen_t len = sizeof(so_error);
        if (getsockopt(sock, SOL_SOCKET, SO_ERROR, &so_error, &len) < 0) {
            std::cerr << "getsockopt failed" << std::endl;
            close(sock);
            return false;
        }
        if (so_error == 0) {
            close(sock);
            return true; // Connection successful
        } else {
            std::cerr << "Connection error: " << so_error << std::endl;
        }
    } else {
        std::cerr << "Connection timed out" << std::endl;
    }

    close(sock);
    return false; // Connection failed
}

int main() {
    std::string host = "10.0.1.115"; // Replace with your server
    int port = 2030; // Replace with your port
    int timeout_ms = 1000; // 1 second timeout

    if (test_socket_connection(host, port, timeout_ms)) {
        std::cout << "Connection successful!" << std::endl;
        // Proceed with httplib::Client request
    } else {
        std::cout << "Connection failed or timed out." << std::endl;
    }

    return 0;
}
