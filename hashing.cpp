#include "hashing.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

// ---------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------

/**
 * Salt appended to every string before hashing.
 * Changing this value invalidates all stored password hashes.
 */
static const std::string HASH_SALT = "Huskr2132";

// ---------------------------------------------------------------------------
// djb2_hash
// ---------------------------------------------------------------------------

unsigned long long djb2_hash(const std::string& str) {
    // DJB2 starting seed — chosen empirically for good distribution
    unsigned long long hash = 5381;

    // Append salt so the hash differs from a plain (unsalted) DJB2 digest
    const std::string salted = str + HASH_SALT;

    for (char c : salted) {
        // Core DJB2 step: hash = hash * 33 + c
        hash = ((hash << 5) + hash) + static_cast<unsigned char>(c);
    }

    return hash;
}

// ---------------------------------------------------------------------------
// CompareHash
// ---------------------------------------------------------------------------

int CompareHash(const std::string& username, const std::string& password) {
    // Pre-compute the hashes of the supplied credentials once
    const unsigned long long usernameHash = djb2_hash(username);
    const unsigned long long passwordHash = djb2_hash(password);

    std::ifstream passFile("password.txt");

    if (!passFile.is_open()) {
        std::cerr << "[Auth] ERROR: Could not open password.txt\n";
        return 0;
    }

    std::string line;
    while (std::getline(passFile, line)) {
        if (line.empty()) continue; // skip blank lines

        std::istringstream ss(line);
        std::string storedUserHash, storedPassHash;
        int roleIndex = 0;

        // Each line: <username_hash> <password_hash> <role_index>
        if (!(ss >> storedUserHash >> storedPassHash >> roleIndex)) {
            continue; // skip malformed lines
        }

        const unsigned long long fileUserHash = std::stoull(storedUserHash);
        const unsigned long long filePassHash = std::stoull(storedPassHash);

        // Both username AND password must match
        if (fileUserHash == usernameHash && filePassHash == passwordHash) {
            if (roleIndex >= 1 && roleIndex <= 3) {
                return roleIndex; // valid role index found
            }
        }
    }

    // No matching credentials found
    return 0;
}