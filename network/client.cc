#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <thread>

#include "socket.h"

int main(int argc, char* argv[]) {
    if (argc < 6) {
        std::cout << "Usage: {master_ip} {master_port} {src_ip} {dst_ip} {data}" << std::endl;
        std::cout << "Example: ./client 20.121.137.95 11000 1" << std::endl;
        return -1;
    }

    std::string master_ip(argv[1]);
    int master_port = atoi(argv[2]);
    std::string src_ip(argv[3]);
    std::string dst_ip(argv[4]);
    int data = atoi(argv[5]);

    VNS::Socket sock;

    int master_sock = sock.Connect(master_ip, master_port);
    if (master_sock == -1) {
        std::cout << "Master connection failure" << std::endl;
        return -1;
    }

    data = 5;  // TODO: delete, for testing
    while (true) {
        std::stringstream ss;
        ss << master_ip << " " << master_port << " " << src_ip << " " << dst_ip << " " << data;

        std::string message = ss.str();
        std::cout << "Sending: " << message << std::endl;

        sock.Send(message);

        data--;  // TODO: delete, for testing
        if (data < -1) break;

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    sock.Close();

    // send(sock, numbers, sizeof(numbers), 0);

    // int result;
    // read(sock, &result, sizeof(result));

    // std::cout << "Received result from server: " << result << std::endl;

    // close(sock);
    return 0;
}
