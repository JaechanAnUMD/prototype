#include <iostream>
#include <string>
#include <unordered_map>

namespace VNS {

class BiDirectionalMap {
private:
    std::unordered_map<std::string, int> ipToNodeId;
    std::unordered_map<int, std::string> nodeIdToIp;

public:
    // Insert a key-value pair
    void Insert(const std::string& ip, int nodeId) {
        ipToNodeId[ip] = nodeId;
        nodeIdToIp[nodeId] = ip;
    }

    // Remove a key-value pair
    void RemoveByIp(const std::string& ip) {
        auto it = ipToNodeId.find(ip);
        if (it != ipToNodeId.end()) {
            int nodeId = it->second;
            ipToNodeId.erase(it);
            nodeIdToIp.erase(nodeId);
        }
    }

    void RemoveByNodeId(int nodeId) {
        auto it = nodeIdToIp.find(nodeId);
        if (it != nodeIdToIp.end()) {
            std::string ip = it->second;
            nodeIdToIp.erase(it);
            ipToNodeId.erase(ip);
        }
    }

    // Get node_id by IP
    int GetNodeId(const std::string& ip) const {
        auto it = ipToNodeId.find(ip);
        if (it != ipToNodeId.end()) {
            return it->second;
        }
        throw std::runtime_error("IP not found");
    }

    // Get IP by node_id
    std::string GetIp(int nodeId) const {
        auto it = nodeIdToIp.find(nodeId);
        if (it != nodeIdToIp.end()) {
            return it->second;
        }
        throw std::runtime_error("Node ID not found");
    }

    // Print all mappings (for debugging)
    void PrintMappings() const {
        std::cout << "IP to Node ID:\n";
        for (const auto& pair : ipToNodeId) {
            std::cout << "  " << pair.first << " -> " << pair.second << "\n";
        }
        std::cout << "Node ID to IP:\n";
        for (const auto& pair : nodeIdToIp) {
            std::cout << "  " << pair.first << " -> " << pair.second << "\n";
        }
    }
};

}  // namespace VNS
