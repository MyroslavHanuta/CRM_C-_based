/**
 * hashing.h
 * Password hashing and authentication declarations.
 *
 * Provides a salted DJB2 hash function and a credential comparison
 * routine that reads hashed credentials from "password.txt".
 *
 * File format of password.txt (one line per user):
 *   <username_hash> <password_hash> <role_index>
 *
 * Role indices:
 *   1 = Admin
 *   2 = Owner
 *   3 = Staff
 */

#ifndef ASSIGNMENT_HASHING_H
#define ASSIGNMENT_HASHING_H

#include <string>

/**
 * Computes a salted DJB2 hash of a string.
 *
 * A fixed salt is appended to the input before hashing so that
 * identical plaintext values produce a different digest than an
 * unsalted hash, improving resistance to pre-computed lookup tables.
 *
 * The plaintext string to hash.
 * The 64-bit hash value.
 */
unsigned long long djb2_hash(const std::string& str);

/**
 * Validates a username/password pair against stored hashes.
 *
 * Opens "password.txt", iterates through each line, hashes the
 * supplied credentials, and compares them to the stored values.
 *
 * @param username  Plaintext username entered by the user.
 * @param password  Plaintext password entered by the user.
 * @return  Role index (1, 2, or 3) on success; 0 if no match found.
 */
int CompareHash(const std::string& username, const std::string& password);

#endif // ASSIGNMENT_HASHING_H