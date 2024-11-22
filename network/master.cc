#include <cstring>
#include <iostream>
#include <sstream>
#include <thread>
#include <vector>

#include "network.h"
#include "node_conn.h"
#include "node_ip.h"
#include "postgres.h"
#include "socket.h"
#include "util.h"

#define BUFFER_SIZE 1024

void nodeThread(VNS::Socket& socket);
void clientThread(VNS::Socket& socket);
void handleClient(int sock);

int main(int argc, char* argv[]) {
    if (argc > 1) {
        std::cout << "Usage: ./master" << std::endl;
        return -1;
    }

    /**
     * Read from Postgre to setup the nodes.
     */
    VNS::Postgres postgres;

    postgres.Open();

    const char* query = "SELECT * FROM node_ips;";
    PGresult* res = postgres.Exec(query);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "Query failed: " << PQerrorMessage(postgres.conn()) << std::endl;
        PQclear(res);
        PQfinish(postgres.conn());
        return 1;
    }

    // Fetch rows and populate the vector
    std::vector<VNS::NodeIp> node_ips;
    int rows = PQntuples(res);

    for (int i = 0; i < rows; ++i) {
        int node_id = std::stoi(PQgetvalue(res, i, PQfnumber(res, "node_id")));
        std::string ip = PQgetvalue(res, i, PQfnumber(res, "ip"));
        int port = std::stoi(PQgetvalue(res, i, PQfnumber(res, "port")));

        node_ips.emplace_back(node_id, ip, port);  // Add to the vector
    }

    // Display the loaded data
    for (const auto& node_ip : node_ips) {
        node_ip.display();
    }

    PQclear(res);

    postgres.Close();

    /**
     * Setup sockets
     */
    int port = node_ips[0].port;  // Master node's index is always 0

    VNS::Socket socket;  // Socket for receiving data from clients

    int result = socket.Open(port);
    if (result == -1) {
        std::cout << "Socket open failed" << std::endl;
        return -1;
    }

    std::vector<std::thread> threads;

    // Handle connections to nodes
    // threads.emplace_back(nodeThread, std::ref(sock));

    // Accept clients
    // TODO: this can be done only after the nodes are fully connected.
    // i.e., clients need to wait until the system is ready.
    threads.emplace_back(clientThread, std::ref(socket));

    while (true) {
        // TODO: run condition
    }

    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }

#if 0
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
#endif

    return 0;
}

void nodeThread(VNS::Socket& socket) {}

void clientThread(VNS::Socket& socket) {
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
