#ifndef MERKLE_TREE_H
#define MERKLE_TREE_H

#include <string>

namespace VNS {

constexpr size_t MAX_ENTRIES = 1024;
constexpr size_t HASH_SIZE = 64;
constexpr size_t IP_SIZE = 16;
constexpr size_t VALUE_SIZE = 32;

struct KeyValue {
    char src_ip[IP_SIZE];
    char dst_ip[IP_SIZE];
    char value[VALUE_SIZE];
    char hash[HASH_SIZE];
};

struct SharedMerkleTree {
    char root_hash[HASH_SIZE];
    KeyValue entries[MAX_ENTRIES];
    size_t count;
};

std::string sha256(const std::string& data);

void merkle_insert_or_update(SharedMerkleTree* tree, const std::string& src_ip, const std::string& dst_ip,
                             const std::string& value);

bool merkle_lookup(const SharedMerkleTree* tree, const std::string& src_ip, const std::string& dst_ip,
                   std::string& value, size_t& ip_count);

}  // namespace VNS

#endif  // MERKLE_TREE_H
