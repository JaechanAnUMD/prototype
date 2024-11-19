#include <iostream>
#include <cstring>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "network.h"
#include "merkle/merklecpp.h"
#include "merkle_tree.h"

#define PORT 11000

void InitRoutingTable(MerkleTree& tree) {
    // Insert some key-value pairs
    tree.insert("{20.121.137.95,172.210.11.93}", {"48.217.241.39", "172.210.11.93"});
    tree.insert("{20.121.137.95,48.217.241.39}", {"172.210.11.93", "48.217.241.39"});

    // Print the root hash
    std::cout << "Root Hash: " << tree.getRootHash() << std::endl;

    // Get a value
    auto value = tree.get("{20.121.137.95,172.210.11.93}");
    std::cout << "Value: " << value[0] << ", " << value[1] << std::endl;
}

int queryNode(const std::string& ip, const int port, int data) {
    int sock = 0;
    struct sockaddr_in serv_addr;
    int result;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation error" << std::endl;
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    // Convernodet IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection Failed" << std::endl;
        return -1;
    }

    send(sock, &data, sizeof(data), 0);

    read(sock, &result, sizeof(result));

    close(sock);

    return result;
}


int main(int argc, char* argv[]) {
#if 0
    merkle::Tree::Hash hash("fa8f44eabb728d4020e7f33d1aa973faaef19de6c06679bccdc5100a3c01f54a");

    merkle::Tree tree;
    tree.insert(hash);
    auto root = tree.root();
    auto path = tree.path(0);
    assert(path->verify(root));
#endif
    MerkleTree routing_table;

    InitRoutingTable(routing_table);

    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Create socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cerr << "Socket failed" << std::endl;
        return -1;
    }

    // Forcefully attach socket to the port 11000
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        std::cerr << "Setsockopt failed" << std::endl;
        return -1;
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket to the network address and port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        return -1;
    }
    if (listen(server_fd, 3) < 0) {
        std::cerr << "Listen failed" << std::endl;
        return -1;
    }

    std::cout << "Server is listening on port " << PORT << std::endl;

    while (true) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            std::cerr << "Accept failed" << std::endl;
            continue;
        }

        // Read integers from the client
        // numbers: [{src}, {dst}, {query_type}]
        int numbers[3];
        read(new_socket, numbers, sizeof(numbers));

        if (numbers[2] == -1) {
            std::cout << "Terminating server" << std::endl;
            send(new_socket, &numbers[2], sizeof(int), 0);
            break;
        }

        // Fetch the ips for the nodes in the routing path
        auto ips = routing_table.get("{20.121.137.95,172.210.11.93}");
        std::cout << "Value: " << ips[0] << ", " << ips[1] << std::endl;

        // Logic for computing final result
        int accumulatedResult = 0;
        for (const auto& ip : ips) {
            std::cout << "Retrieving data from " << ip << ":" << PORT << std::endl;
            accumulatedResult += queryNode(ip, PORT, accumulatedResult);
        }

        std::cout << "Final result from path traversal: " << accumulatedResult << std::endl;
        // Perform operation (e.g., sum of the integers)
        // int result = numbers[0] + numbers[1];
        // std::cout << "Received numbers: " << numbers[0] << " and " << numbers[1] << ". Sending result: " << result << std::endl;

        // Send the result back to the client
        send(new_socket, &accumulatedResult, sizeof(accumulatedResult), 0);

        close(new_socket);
    }

    close(server_fd);
    return 0;
}
