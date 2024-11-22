#include "merkle_tree.h"

#include <fcntl.h>
#include <openssl/sha.h>
#include <sys/mman.h>

#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace VNS {

std::string sha256(const std::string& data) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(data.c_str()), data.size(), hash);
    std::ostringstream result;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        result << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }
    return result.str();
}

void merkle_insert_or_update(SharedMerkleTree* tree, const std::string& src_ip, const std::string& dst_ip,
                             const std::string& value) {
    for (size_t i = 0; i < tree->count; ++i) {
        if (strncmp(tree->entries[i].src_ip, src_ip.c_str(), IP_SIZE) == 0 &&
            strncmp(tree->entries[i].dst_ip, dst_ip.c_str(), IP_SIZE) == 0) {
            // Update value
            strncpy(tree->entries[i].value, value.c_str(), VALUE_SIZE);
            std::string leaf_data = src_ip + dst_ip + value;
            strncpy(tree->entries[i].hash, sha256(leaf_data).c_str(), HASH_SIZE);
            return;
        }
    }

    if (tree->count < MAX_ENTRIES) {
        strncpy(tree->entries[tree->count].src_ip, src_ip.c_str(), IP_SIZE);
        strncpy(tree->entries[tree->count].dst_ip, dst_ip.c_str(), IP_SIZE);
        strncpy(tree->entries[tree->count].value, value.c_str(), VALUE_SIZE);
        std::string leaf_data = src_ip + dst_ip + value;
        strncpy(tree->entries[tree->count].hash, sha256(leaf_data).c_str(), HASH_SIZE);
        tree->count++;
    }
}

bool merkle_lookup(const SharedMerkleTree* tree, const std::string& src_ip, const std::string& dst_ip,
                   std::string& value, size_t& ip_count) {
    for (size_t i = 0; i < tree->count; ++i) {
        if (strncmp(tree->entries[i].src_ip, src_ip.c_str(), IP_SIZE) == 0 &&
            strncmp(tree->entries[i].dst_ip, dst_ip.c_str(), IP_SIZE) == 0) {
            value = std::string(tree->entries[i].value);

            // Check if the value contains one or two IPs
            ip_count = (value.find(';') != std::string::npos) ? 2 : 1;
            return true;
        }
    }
    return false;  // Key not found
}

}  // namespace VNS
