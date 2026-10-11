#pragma once

#include <string>

// Pure C++17 implementation of standard FIPS 180-2 SHA-256 hashing.
// Keeps the application completely self-contained with no external OpenSSL dependency.
namespace crypto {

std::string sha256(const std::string& input);
std::string generateSalt(std::size_t length = 16);

} // namespace crypto
