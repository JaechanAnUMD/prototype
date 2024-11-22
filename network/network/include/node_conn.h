#ifndef NODE_CONN_H
#define NODE_CONN_H

#include <iostream>
#include <string>

namespace VNS {

class NodeConn {
public:
    int from_node_id;
    int to_node_id;

    NodeConn(int fid, int tid) : from_node_id(fid), to_node_id(tid) {}

    void display() const {
        std::cout << "From Node ID: " << from_node_id << ", To Node ID: " << to_node_id << std::endl;
    }
};

}  // namespace VNS

#endif  // NODE_CONN_H
