#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <sstream>
#include <thread>
#include <vector>

#include "merkle_tree.h"
#include "network.h"
#include "node_conn.h"
#include "node_ip.h"
#include "postgres.h"
#include "route.h"
#include "socket.h"
#include "util.h"

#define BUFFER_SIZE 1024

void nodeThread(VNS::Socket& socket);
void clientThread(VNS::Socket& socket);
void handleClient(int sock);

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: ./master {port}" << std::endl;
        return -1;
    }

    /**
     * Read from Postgres to setup the routing table.
     */
    VNS::Postgres postgres;

    postgres.Open();

    const char* query = "SELECT * FROM routes;";
    PGresult* res = postgres.Exec(query);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "Query failed: " << PQerrorMessage(postgres.conn()) << std::endl;
        PQclear(res);
        PQfinish(postgres.conn());
        return 1;
    }

    std::vector<VNS::Route> routes;

    int rows = PQntuples(res);
    for (int i = 0; i < rows; ++i) {
        std::string src_ip = PQgetvalue(res, i, PQfnumber(res, "src_ip"));
        std::string dst_ip = PQgetvalue(res, i, PQfnumber(res, "dst_ip"));
        std::string node_ip = PQgetvalue(res, i, PQfnumber(res, "node_ip"));
        routes.emplace_back(src_ip, dst_ip, node_ip);
    }

    PQclear(res);

    postgres.Close();

    /**
     * Create Merkle tree in a shared memory location
     */
    // Create shared memory
    int shm_fd = shm_open("/merkle_tree_shm", O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        return 1;
    }

    // Resize shared memory
    if (ftruncate(shm_fd, sizeof(VNS::SharedMerkleTree)) == -1) {
        perror("ftruncate");
        return 1;
    }

    // Map shared memory
    void* shm_ptr = mmap(0, sizeof(VNS::SharedMerkleTree), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    auto* tree = static_cast<VNS::SharedMerkleTree*>(shm_ptr);
    memset(tree, 0, sizeof(VNS::SharedMerkleTree));

    // Create a Merkle tree of the routing table
    for (const auto& route : routes) {
        std::string value;
        size_t ip_count = 0;

        bool found = merkle_lookup(tree, route.src_ip, route.dst_ip, value, ip_count);

        std::string new_value = value;
        if (found) {
            new_value = new_value + ";" + route.node_ip;
        } else {
            new_value = route.node_ip;
        }
        merkle_insert_or_update(tree, route.src_ip, route.dst_ip, new_value);
    }

    /**
     * Setup sockets
     */
    int port = atoi(argv[1]);

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

    // Cleanup
    munmap(shm_ptr, sizeof(SharedMerkleTree));
    shm_unlink("/merkle_tree_shm");

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
