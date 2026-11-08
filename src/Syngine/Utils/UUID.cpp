// ╒═════════════════════════ UUID.cpp ═╕
// │ Syngine                              │
// │ Created 2026-11-07                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Licensed under the MIT License       │
// ╰──────────────────────────────────────╯

#include "Syngine/Utils/UUID.h"

#include <array>
#include <random>

namespace Syngine {

Syngine::UUID GenerateUUID() {
    thread_local std::mt19937_64 generator([] {
        std::random_device source;
        std::seed_seq      seed{ source(), source(), source(), source(),
                                 source(), source(), source(), source() };
        return std::mt19937_64(seed);
    }());

    std::array<unsigned char, 16> bytes;
    for (size_t i = 0; i < bytes.size(); i += 8) {
        auto value = generator();
        for (size_t j = 0; j < 8; ++j) {
            bytes[i + j] = static_cast<unsigned char>(value >> (j * 8));
        }
    }
    bytes[6] = static_cast<unsigned char>((bytes[6] & 0x0f) | 0x40);
    bytes[8] = static_cast<unsigned char>((bytes[8] & 0x3f) | 0x80);

    std::array<unsigned char, 16> result;
    std::copy(bytes.begin(), bytes.end(), result.begin());
    return UUID(result);
}

Syngine::UUID UUIDFromString(const std::string& str) {
    if (str.size() != 36) {
        throw std::invalid_argument("Invalid UUID string length");
    }

    unsigned int data[16];
    if (sscanf(str.c_str(),
               "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%"
               "02x%02x",
               &data[0],
               &data[1],
               &data[2],
               &data[3],
               &data[4],
               &data[5],
               &data[6],
               &data[7],
               &data[8],
               &data[9],
               &data[10],
               &data[11],
               &data[12],
               &data[13],
               &data[14],
               &data[15]) != 16) {
        throw std::invalid_argument("Invalid UUID string format");
    }

    std::array<unsigned char, 16> bytes;
    for (size_t i = 0; i < 16; ++i) {
        bytes[i] = static_cast<unsigned char>(data[i]);
    }

    return UUID(bytes);
}

} // namespace Syngine
