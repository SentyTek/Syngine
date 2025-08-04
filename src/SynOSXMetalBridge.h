#ifndef SYN_METAL_BRIDGE_H
#define SYN_METAL_BRIDGE_H

#include <SDL3/SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

/// @brief Get the Metal view for a given SDL window on macOS
/// @param win The SDL window for which to get the Metal view
/// @return A pointer to the Metal view, or nullptr if not available
/// @note This function is specific to macOS and uses Objective-C to retrieve
/// the Metal view from the SDL window.
/// @since v0.0.1
/// @internal
void* _GetSYNMetalView(SDL_Window* win);

#ifdef __cplusplus
}
#endif

#endif // SynOSXMetalBridge_h