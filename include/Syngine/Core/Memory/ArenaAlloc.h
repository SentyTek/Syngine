// ╒═══════════════════════ ArenaAlloc.h ═╕
// │ Syngine                              │
// │ Created 2026-08-20                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Licensed under the MIT License       │
// ╰──────────────────────────────────────╯

#pragma once

#include <cstddef>
#include <span>
#include <utility>
#include <Syngine/Core/Logger.h>

namespace Syngine::Memory {

class ArenaAlloc {
    std::byte* m_buffer;
    size_t     m_capacity;
    size_t     m_offset;

  public:
    explicit ArenaAlloc();

    /// @brief Creates an arena allocator with the specified capacity.
    /// @param capacity The total capacity of the arena in bytes.
    /// @since v0.0.2
    explicit ArenaAlloc(size_t capacity);
    ~ArenaAlloc();

    // Owns a raw buffer; copying would double-free, so only allow moves.
    ArenaAlloc(const ArenaAlloc&)            = delete;
    ArenaAlloc& operator=(const ArenaAlloc&) = delete;

    ArenaAlloc(ArenaAlloc&& other) noexcept;
    ArenaAlloc& operator=(ArenaAlloc&& other) noexcept;

    /// @brief Allocates a block of memory from the arena.
    /// @param size The size of the memory block in bytes.
    /// @param alignment The alignment requirement for the memory block.
    /// @return A pointer to the allocated memory, or nullptr if allocation
    /// fails.
    /// @since v0.0.2
    template <typename T>
    std::span<T> Allocate(size_t count, size_t alignment = alignof(T)) {
        size_t size           = sizeof(T) * count;
        size_t currentAddress = reinterpret_cast<size_t>(m_buffer) + m_offset;
        size_t padding = (alignment - (currentAddress % alignment)) % alignment;
        if (m_offset + padding + size > m_capacity) {
            Logger::LogF(LogLevel::ERR,
                         true,
                         "ArenaAlloc: Out of memory. Requested %zu, capacity "
                         "%zu, offset %zu",
                         size,
                         m_capacity,
                         m_offset);
            return std::span<T>();
        }
        m_offset += padding;
        T* ptr = reinterpret_cast<T*>(m_buffer + m_offset);
        m_offset += size;
        return std::span<T>(ptr, count);
    };

    /// @brief Resets the arena allocator, making all previously allocated
    /// memory available again.
    /// @since v0.0.2
    void Reset();

    /// @brief Creates an object of type T in the arena allocator.
    /// @tparam T The type of the object to create.
    /// @tparam Args The types of the constructor arguments.
    /// @param args The constructor arguments for the object.
    /// @return A pointer to the created object, or nullptr if allocation fails.
    /// @since v0.0.2
    template <typename T, typename... Args> T* Create(Args&&... args) {
        auto memory = Allocate<T>(1, alignof(T));
        if (memory.empty()) return nullptr;
        return std::construct_at(memory.data(), std::forward<Args>(args)...);
    }
};

} // namespace Syngine::Memory
