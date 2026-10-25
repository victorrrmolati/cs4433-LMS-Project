#include "PasswordHasher.h"
#include <windows.h> 
#include <argon2.h>
#include <bcrypt.h>

#include <stdexcept>
#include <vector>
#include <cstdint>

using namespace std;

namespace {
    constexpr uint32_t TIME_COST = 3;
    constexpr uint32_t MEMORY_COST = 65536; // 64 MiB
    constexpr uint32_t PARALLELISM = 1;
    constexpr uint32_t SALT_LENGTH = 16;
    constexpr uint32_t HASH_LENGTH = 32;
}

string PasswordHasher::hashPassword(const string& password) {
    if (password.empty()) {
        throw invalid_argument("Password cannot be empty.");
    }

    // Generate a cryptographically secure random salt using Windows.
    vector<unsigned char> salt(SALT_LENGTH);

    NTSTATUS status = BCryptGenRandom(
        nullptr,
        salt.data(),
        static_cast<ULONG>(salt.size()),
        BCRYPT_USE_SYSTEM_PREFERRED_RNG
    );

    if (status < 0) {
        throw runtime_error("Secure salt generation failed.");
    }

    // Allocate enough space for the encoded Argon2id hash.
    size_t encodedLength = argon2_encodedlen(
        TIME_COST,
        MEMORY_COST,
        PARALLELISM,
        SALT_LENGTH,
        HASH_LENGTH,
        Argon2_id
    );

    vector<char> encodedHash(encodedLength);

    int result = argon2id_hash_encoded(
        TIME_COST,
        MEMORY_COST,
        PARALLELISM,
        password.data(),
        password.size(),
        salt.data(),
        salt.size(),
        HASH_LENGTH,
        encodedHash.data(),
        encodedHash.size()
    );

    if (result != ARGON2_OK) {
        throw runtime_error(
            string("Password hashing failed: ") +
            argon2_error_message(result)
        );
    }

    return string(encodedHash.data());
}

bool PasswordHasher::verifyPassword(
    const string& password,
    const string& storedHash
) {
    if (password.empty() || storedHash.empty()) {
        return false;
    }

    int result = argon2id_verify(
        storedHash.c_str(),
        password.data(),
        password.size()
    );

    if (result == ARGON2_OK) {
        return true;
    }

    if (result == ARGON2_VERIFY_MISMATCH) {
        return false;
    }

    throw runtime_error(
        string("Password verification failed: ") +
        argon2_error_message(result)
    );
}