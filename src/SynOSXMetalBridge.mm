#ifdef __OBJC__
#import <Cocoa/Cocoa.h>
#endif

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

//export C style function
extern "C" void* GetSYNMetalView(SDL_Window* win) {
    SDL_PropertiesID props = SDL_GetWindowProperties(win);
    if (!props) return nullptr;

    void* nsWindowPtr = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, NULL);
    int64_t tag = SDL_GetNumberProperty(props, SDL_PROP_WINDOW_COCOA_METAL_VIEW_TAG_NUMBER, 0);
    if (!nsWindowPtr || tag == 0) return nullptr;

    NSWindow* nsWindow = (__bridge NSWindow*)nsWindowPtr;
    NSView* contentView = [nsWindow contentView];

    NSLog(@"Metal tag: %lld", tag);
    NSLog(@"Subviews of contentView:");
    for (NSView* view in contentView.subviews) {
        NSLog(@"  → %@ (tag: %ld)", view, view.tag);
    }

    NSView* metalView = FindTaggedView(contentView, tag);

    if (!metalView) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to get Metal view from window");
        return nullptr;
    }

    return (__bridge void*)metalView; //please please work 
}