#ifndef ROUTE_H
#define ROUTE_H

#include <iostream>
#include <string>

namespace VNS {

class Route {
public:
    std::string src_ip;
    std::string dst_ip;
    std::string node_ip;

    Route(const std::string& sip, const std::string& dip, const std::string& nip)
        : src_ip(sip), dst_ip(dip), node_ip(nip) {}

    void display() const {
        std::cout << "Src IP: " << src_ip << ", Dst IP: " << dst_ip << ", Node IP: " << node_ip << std::endl;
    }
};

}  // namespace VNS

#endif  // ROUTE_H
