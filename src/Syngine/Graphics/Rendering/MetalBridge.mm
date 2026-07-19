// ╒═════════════════════ MetalBridge.mm ═╕
// │ Syngine                              │
// │ Created 2025-04-22                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// | Licensed under the MIT License       |
// ╰──────────────────────────────────────╯

#include "SDL3/SDL_video.h"
#ifdef __OBJC__
#import <Cocoa/Cocoa.h>
#endif

#include "Syngine/Core/Logger.h"
#include <stdint.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_metal.h>

static NSView* FindTaggedView(NSView* root, NSInteger tag) {
    if (root.tag == tag) return root;
    if (!root.subviews || root.subviews.count == 0) {
        Syngine::Logger::LogF(
            Syngine::LogLevel::ERR,
            false,
            "View with tag %ld has no subviews. Renderer cannot be created.",
            (long)tag);
        return nil;
    }

    for (NSView* subview in root.subviews) {
        NSView* result = FindTaggedView(subview, tag);
        if (result) return result;
    }
    return nil;
}

/// This function is called from C code to get the Metal view associated with a given SDL window.
/// MacOS Exclusive (Thanks Tim Apple!)
extern "C" void* _GetSYNMetalView(SDL_Window* win) {
    SDL_PropertiesID props = SDL_GetWindowProperties(win);
    if (!props) return nullptr;

    // First try to explicitly create or fetch the Metal view from SDL.
    // This guarantees a CAMetalLayer-backed NSView and avoids relying on tags.
    SDL_MetalView metalView = SDL_Metal_CreateView(win);
    if (metalView) {
        return metalView; // Already a NSView* castable to void*
    }

    // Fallbacks (older SDLs), try the tag-based lookup if present
    void* nsWindowPtr = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, NULL);
    int64_t tag = SDL_GetNumberProperty(props, SDL_PROP_WINDOW_COCOA_METAL_VIEW_TAG_NUMBER, 0);
    if (!nsWindowPtr || tag <= 0 || tag == 255) {
        Syngine::Logger::LogF(
            Syngine::LogLevel::ERR,
            false,
            "Failed to get valid Metal view (tag=%ld, win=%p)",
            (long)tag,
            nsWindowPtr);
        // As a last resort, return the content view; bgfx may attach a layer.
        void* cocoaView = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, NULL);
        return cocoaView;
    }

    NSWindow* nsWindow = (__bridge NSWindow*)nsWindowPtr;
    NSView* contentView = [nsWindow contentView];
    NSView* found = FindTaggedView(contentView, (NSInteger)tag);
    if (!found) {
        Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                              false,
                              "Failed to find Metal subview by tag");
        void* cocoaView = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, NULL);
        return cocoaView;
    }

    return (__bridge void*)found;
}
