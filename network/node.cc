#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "int_merkle_tree.h"

void InitRoutingTable(IntMerkleTree& tree) {
    tree.insert("{20.121.137.95,172.210.11.93}", 40);
    tree.insert("{20.121.137.95,48.217.241.39}", 100);
}

int performCalculation(int data) {
    return data + 1; // Example calculation (customize as needed)
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cout << "Usage: {port}" << std::endl;
        std::cout << "Example: ./node 11000" << std::endl;
        return -1;
    }

    IntMerkleTree routing_table;

    InitRoutingTable(routing_table);

    int port = atoi(argv[1]);
    int node_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Create socket file descriptor
    if ((node_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cerr << "Socket failed" << std::endl;
        return -1;
    }

    // Forcefully attach socket to the port 11000
    if (setsockopt(node_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        std::cerr << "Setsockopt failed" << std::endl;
        return -1;
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    // Bind the socket to the network address and port
    if (bind(node_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        return -1;
    }

    if (listen(node_fd, 3) < 0) {
        std::cerr << "Listen failed" << std::endl;
        return -1;
    }

    std::cout << "Node is listening on port " << port << std::endl;

    while (true) {
        if ((new_socket = accept(node_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            std::cerr << "Accept failed" << std::endl;
            continue;
        }

        int receivedData;
        int bytesRead = read(new_socket, &receivedData, sizeof(receivedData));
        if (bytesRead <= 0) {
            std::cerr << "Failed to read data from master" << std::endl;
            close(new_socket);
            continue;
        }

        std::cout << "Node received: " << receivedData << std::endl;

        int result = performCalculation(receivedData);
        send(new_socket, &result, sizeof(result), 0);

        std::cout << "Node sent: " << result << std::endl;

        close(new_socket);
    }

    close(node_fd);
    return 0;
}
