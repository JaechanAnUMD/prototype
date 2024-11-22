#include <cstring>
#include <iostream>
#include <sstream>
#include <thread>
#include <vector>

#include "network.h"
#include "socket.h"
#include "util.h"

#define BUFFER_SIZE 1024

void handleClient(int sock);

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: ./master {port}" << std::endl;
        return -1;
    }

    int port = atoi(argv[1]);

    VNS::Socket socket;

    int result = socket.Open(port);
    if (result == -1) {
        std::cout << "Socket open failed" << std::endl;
        return -1;
    }

    VNS::BiDirectionalMap node_ips;

    std::vector<std::thread> threads;
    while (true) {
        int client_sock = socket.Accept();

        if (client_sock == -1) {
            std::cout << "Accept failed" << std::endl;
            continue;
        }

        std::cout << "New client connected" << std::endl;

        // Create a thread to handle the client
        threads.emplace_back(handleClient, client_sock);
    }

    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }

    socket.Close();

    return 0;
}

void handleClient(int sock) {
    char buffer[BUFFER_SIZE];
    VNS::Socket socket(sock);

    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        ssize_t bytes_read = socket.Read(buffer, BUFFER_SIZE - 1);

        if (bytes_read <= 0) {
            continue;
        }

        std::vector<std::string> parsed;
        std::stringstream ss(buffer);
        std::string token;

        while (getline(ss, token, ' ')) {
            parsed.push_back(token);
        }

        if (parsed.size() < 5) {
            std::cerr << "Client sent a wrong data" << std::endl;
            socket.Close();
            return;
        }

        int data = atoi(parsed[2].c_str());

        // Termination condition
        if (data < 0) {
            std::cout << "Terminating client" << std::endl;
            socket.Close();
            return;
        }

        std::cout << "Received from client: " << buffer << std::endl;

        // TODO: parse client's data and send to nodes
        // {src_ip} {dst_ip} {data}
        // {string} {string} {int }
    }
}
