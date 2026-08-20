// ╒════════════════════════════ ArenaAlloc.cpp ═╕
// │ Syngine                              │
// │ Created 2026-08-20                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Licensed under the MIT License       │
// ╰──────────────────────────────────────╯

#include <Syngine/Core/Memory/ArenaAlloc.h>

namespace Syngine::Memory {

ArenaAlloc::ArenaAlloc() : m_buffer(nullptr), m_capacity(0), m_offset(0) {}

ArenaAlloc::ArenaAlloc(size_t capacity)
    : m_buffer(static_cast<std::byte*>(::operator new(capacity))),
      m_capacity(capacity), m_offset(0) {}

ArenaAlloc::~ArenaAlloc() { ::operator delete(m_buffer); }

ArenaAlloc::ArenaAlloc(ArenaAlloc&& other) noexcept
    : m_buffer(other.m_buffer), m_capacity(other.m_capacity),
      m_offset(other.m_offset) {
    other.m_buffer   = nullptr;
    other.m_capacity = 0;
    other.m_offset   = 0;
}

ArenaAlloc& ArenaAlloc::operator=(ArenaAlloc&& other) noexcept {
    if (this != &other) {
        ::operator delete(m_buffer);
        m_buffer   = other.m_buffer;
        m_capacity = other.m_capacity;
        m_offset   = other.m_offset;

        other.m_buffer   = nullptr;
        other.m_capacity = 0;
        other.m_offset   = 0;
    }
    return *this;
}

void ArenaAlloc::Reset() { m_offset = 0; }

} // namespace Syngine::Memory
