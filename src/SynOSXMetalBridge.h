#ifndef SYN_METAL_BRIDGE_H
#define SYN_METAL_BRIDGE_H

#include <SDL3/SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

void* GetSYNMetalView(SDL_Window* win);

#ifdef __cplusplus
}
#endif

#endif // SynOSXMetalBridge_h