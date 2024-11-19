#ifndef INT_MERKLE_TREE_H
#define INT_MERKLE_TREE_H

#include <iostream>
#include <string>
#include <unordered_map>
#include <openssl/sha.h> // For hashing

// Utility function to compute SHA-256 hash
std::string computeHash(const std::string& data) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(data.c_str()), data.size(), hash);

    char hexHash[SHA256_DIGEST_LENGTH * 2 + 1];
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        sprintf(hexHash + (i * 2), "%02x", hash[i]);
    }
    return std::string(hexHash);
}

// Merkle Tree Node for integer values
struct IntMerkleNode {
    std::string hash;
    std::string key;
    int value;
    IntMerkleNode* left;
    IntMerkleNode* right;

    IntMerkleNode(const std::string& k = "", int v = 0)
        : key(k), value(v), left(nullptr), right(nullptr) {
            hash = computeHash(k + std::to_string(v));
        }
};

// Integer Merkle Tree
class IntMerkleTree {
    private:
        IntMerkleNode* root;
        std::unordered_map<std::string, IntMerkleNode*> keyMap; // Fast lookup for keys

        // Helper to calculate hashes recursively
        void recalculateHash(IntMerkleNode* node) {
            if (!node) return;
            if (!node->left && !node->right) {
                node->hash = computeHash(node->key + std::to_string(node->value));
            } else {
                std::string combinedHash = (node->left ? node->left->hash : "") + (node->right ? node->right->hash : "");
                node->hash = computeHash(combinedHash);
            }
        }

        // Recursive insertion helper
        IntMerkleNode* insert(IntMerkleNode* node, const std::string& key, int value) {
            if (!node) {
                auto newNode = new IntMerkleNode(key, value);
                keyMap[key] = newNode;
                return newNode;
            }
            if (key < node->key) {
                node->left = insert(node->left, key, value);
            } else {
                node->right = insert(node->right, key, value);
            }
            recalculateHash(node);
            return node;
        }

        // Recursive deletion helper
        IntMerkleNode* remove(IntMerkleNode* node, const std::string& key) {
            if (!node) return nullptr;
            if (key < node->key) {
                node->left = remove(node->left, key);
            } else if (key > node->key) {
                node->right = remove(node->right, key);
            } else {
                keyMap.erase(key);
                if (!node->left) {
                    IntMerkleNode* rightChild = node->right;
                    delete node;
                    return rightChild;
                } else if (!node->right) {
                    IntMerkleNode* leftChild = node->left;
                    delete node;
                    return leftChild;
                }
                // Find in-order successor
                IntMerkleNode* successor = node->right;
                while (successor->left) successor = successor->left;
                node->key = successor->key;
                node->value = successor->value;
                keyMap[node->key] = node; // Update keyMap
                node->right = remove(node->right, successor->key);
            }
            recalculateHash(node);
            return node;
        }

    public:
        IntMerkleTree() : root(nullptr) {}

        // Insert a key-value pair
        void insert(const std::string& key, int value) {
            root = insert(root, key, value);
        }

        // Delete a key-value pair
        void remove(const std::string& key) {
            root = remove(root, key);
        }

        // Update a key-value pair
        void update(const std::string& key, int newValue) {
            if (keyMap.find(key) == keyMap.end()) {
                std::cerr << "Key not found: " << key << std::endl;
                return;
            }
            keyMap[key]->value = newValue;
            recalculateHash(root);
        }

        // Get a value by key
        int get(const std::string& key) {
            if (keyMap.find(key) == keyMap.end()) {
                std::cerr << "Key not found: " << key << std::endl;
                return -1;
            }
            return keyMap[key]->value;
        }

        // Get the root hash
        std::string getRootHash() const {
            return root ? root->hash : "";
        }
};

#endif // INT_MERKLE_TREE_H
