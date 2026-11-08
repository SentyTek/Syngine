// ╒═════════════════════════════ UUID.h ═╕
// │ Syngine                              │
// │ Created 2026-11-07                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Licensed under the MIT License       │
// ╰──────────────────────────────────────╯

#pragma once

#include <string>
#include <array>

namespace Syngine {

/// @brief Represents a universally unique identifier (UUID) compliant with RFC
/// 4122.
/// @since v0.0.3
struct UUID {
  private:
    uint64_t high;
    uint64_t low;
    friend struct std::hash<Syngine::UUID>;

  public:
    UUID() : high(0), low(0) {}

    explicit UUID(const std::array<unsigned char, 16>& bytes)
        : high(0), low(0) {
        for (size_t i = 0; i < 8; ++i) {
            high = (high << 8) | bytes[i];
        }
        for (size_t i = 8; i < 16; ++i) {
            low = (low << 8) | bytes[i];
        }
    }

    /// @brief Converts the UUID to its canonical lowercase string
    /// representation.
    /// @return The UUID as a string in the format
    /// "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx".
    /// @since v0.0.3
    std::string ToString() const {
        char buffer[37];
        snprintf(buffer,
                 sizeof(buffer),
                 "%08x-%04x-%04x-%04x-%012llx",
                 (unsigned)(high >> 32),
                 (unsigned)(high >> 16) & 0xFFFF,
                 (unsigned)high & 0xFFFF,
                 (unsigned)(low >> 48),
                 (unsigned long long)(low & 0xFFFFFFFFFFFFULL));
        return std::string(buffer);
    }

    bool operator==(const UUID& other) const = default;
    bool operator!=(const UUID& other) const = default;
};

/// @brief Generates a random RFC 4122 version 4 UUID.
/// @return The UUID in canonical lowercase string form.
/// @note The generator is intended for identity, not cryptographic security.
/// @threadsafety safe
/// @since v0.0.3
Syngine::UUID GenerateUUID();

/// @brief Creates a UUID from its string representation.
/// @param str The UUID string
/// @return The parsed UUID.
/// @throws std::invalid_argument if the string is not a valid UUID.
/// @since v0.0.3
Syngine::UUID UUIDFromString(const std::string& str);

} // namespace Syngine

// this is truly some wild shit
namespace std {

template <> struct hash<Syngine::UUID> {
    std::size_t operator()(const Syngine::UUID& uuid) const noexcept {
        return std::hash<uint64_t>()(uuid.high) ^
               std::hash<uint64_t>()(uuid.low);
    }
};

} // namespace std
