#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>


int performCalculation(int data) {
    return data * 2; // Example calculation (customize as needed)
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cout << "Usage: {port}" << std::endl;
        std::cout << "Example: ./node 11001" << std::endl;
        return -1;
    }

    int port = atoi(argv[1]);
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 3);

    std::cout << "Node is listening on port " << port << std::endl;

    while ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) >= 0) {
        int receivedData;
        read(new_socket, &receivedData, sizeof(receivedData));

        int result = performCalculation(receivedData);
        send(new_socket, &result, sizeof(result), 0);

        close(new_socket);
    }

    close(server_fd);

    return 0;
}
