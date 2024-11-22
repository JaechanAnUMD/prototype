#include "socket.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cassert>
#include <iostream>
#include <string>

namespace VNS {

Socket::Socket(int sock) {
    Socket();
    m_sock = sock;
}

int Socket::Open(const int port) {
    int sock;
    struct sockaddr_in address;
    int opt = 1;

    // Create socket file descriptor
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cerr << "Socket failed" << std::endl;
        return -1;
    }

    // Attach socket to the port
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        std::cerr << "Setsockopt failed" << std::endl;
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    // Bind the socket to the network address and port
    if (bind(sock, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        return -1;
    }

    if (listen(sock, 5) < 0) {
        std::cerr << "Listen failed" << std::endl;
        return -1;
    }

    std::cout << "Server is listening on port " << port << std::endl;

    m_sock = sock;
    m_port = port;

    return sock;
}

void Socket::Close() {
    if (m_sock != -1) {
        std::cout << "Server is closing socket " << m_sock << std::endl;
        close(m_sock);
    }
}

int Socket::Accept() {
    assert(Valid());

    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);

    int sock = accept(m_sock, (struct sockaddr*)&address, &addrlen);
    if (sock == -1) {
        return -1;
    }

    return sock;
}

int Socket::Connect(const std::string& ip, const int port) {
    int sock = 0;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket failed" << std::endl;
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        return -1;
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection Failed" << std::endl;
        return -1;
    }

    m_sock = sock;
    m_port = port;
    std::cout << "Before connect" << std::endl;

    return sock;
}

ssize_t Socket::Read(char* buf, ssize_t size) {
    assert(Valid());

    ssize_t bytes_read = read(m_sock, buf, size);

    return bytes_read;
}

void Socket::Send(const std::string& message) {
    assert(Valid());

    send(m_sock, message.c_str(), message.size(), 0);
}

}  // namespace VNS
