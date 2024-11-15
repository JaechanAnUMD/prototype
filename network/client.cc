#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdlib>

#define PORT 11000

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Usage: {source} {destination} {query_type}" << std::endl
            << "Query Type:" << std::endl
            << "1. tmp" << std::endl
            << "2. RTT" << std::endl
            << "3. Packet Loss Rate" << std::endl;
        std::cout << "Example: ./client 1 5 1" << std::endl;
        return -1;
    }

    int src = atoi(argv[1]);
    int dst = atoi(argv[2]);
    int query_type = atoi(argv[3]);

    int sock = 0;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation error" << std::endl;
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection Failed" << std::endl;
        return -1;
    }

    int numbers[] = {src, dst, query_type};
    std::cout << "Source: " << src
        << " Destination: " << dst
        << " Query type: " << query_type << std::endl;

    send(sock, numbers, sizeof(numbers), 0);

    int result;
    read(sock, &result, sizeof(result));

    std::cout << "Received result from server: " << result << std::endl;

    close(sock);
    return 0;
}
