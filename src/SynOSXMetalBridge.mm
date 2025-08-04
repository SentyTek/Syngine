// ╒═══════════════ SynOSXMetalBridge.mm ═╕
// │ Syngine                              │
// │ Created 2025-04-22                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2025     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#ifdef __OBJC__
#import <Cocoa/Cocoa.h>
#endif

#include "SyngineLogger.h"
#include <stdint.h>
#include <SDL3/SDL.h>

static NSView* FindTaggedView(NSView* root, NSInteger tag) {
    if (root.tag == tag) return root;
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

    void* nsWindowPtr = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, NULL);
    int64_t tag = SDL_GetNumberProperty(props, SDL_PROP_WINDOW_COCOA_METAL_VIEW_TAG_NUMBER, 0);
    if (!nsWindowPtr || tag == 0) return nullptr;

    NSWindow* nsWindow = (__bridge NSWindow*)nsWindowPtr;
    NSView* contentView = [nsWindow contentView];

    NSView* metalView = FindTaggedView(contentView, tag);

    if (!metalView) {
        // Must be LogF for Obj-C reasons
        Syngine::Logger::LogF(Syngine::LogLevel::ERR, "Failed to get Metal view from window");
        return nullptr;
    }

    return (__bridge void*)metalView; //please please work 
}