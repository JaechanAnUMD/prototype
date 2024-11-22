#ifndef NODE_IP_H
#define NODE_IP_H

#include <iostream>
#include <string>

namespace VNS {

class NodeIp {
public:
    int node_id;     // node_id column
    std::string ip;  // ip column
    int port;        // port column

    // Constructor
    NodeIp(int id, const std::string& ip_address, int p) : node_id(id), ip(ip_address), port(p) {}

    void display() const { std::cout << "Node ID: " << node_id << ", IP: " << ip << ", Port: " << port << std::endl; }
};

}  // namespace VNS

#endif  // NODE_IP_H
