#pragma once
#include "SyngineGraphics.h"
#include "SynModelLoader.h"

struct SyngineApp {
    SyngineGraphics* graphics;
    SynModelLoader* models;
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