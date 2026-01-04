// ╒══════════════════════════ Helpers.h ═╕
// │ Syngine                              │
// │ Created 2025-05-07                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#pragma once

namespace Syngine {

/// @brief Platform enum to identify the current platform
enum class Platform {
	DARWIN, WINDOWS, LINUX,
};


#if defined(_WIN32)
constexpr Platform CurrentPlatform = Platform::WINDOWS; //* The current platform is Windows
#elif defined(__APPLE__)
constexpr Platform CurrentPlatform = Platform::DARWIN; //* The current platform is macOS
#elif defined(__linux__)
constexpr Platform CurrentPlatform = Platform::LINUX; //* The current platform is Linux
#else
static_assert(false, "Unsupported platform: not defined in SynPlatform");
#endif

} // namespace Syngine
