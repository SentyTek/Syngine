#pragma once
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_hints.h>

#include <bgfx/bgfx.h>

#include <SyngineGraphics.h>

struct SyngineApp {
    SyngineGraphics* graphics; // lets point
};

class SyngineCore {
    public:
    SyngineApp* app;

    SyngineCore();
    ~SyngineCore();
    int AttachGraphics(SyngineGraphics* graphics);
    int DetachGraphics();

    int SyngineEventLoop();
};