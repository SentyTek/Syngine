#pragma once
#include "SyngineGraphics.h"
#include "SynModelLoader.h"
#include "SyngineGameobject.h"
#include "SynginePhys.h"

namespace Syngine {

// Structure to hold hardware specifications
struct HardwareSpecs {
    std::string osName;
    std::string cpuModel;
    std::string cpuArch;
    int         cpuCores;
    int         ramMB;
    int         screenWidth;
    int         screenHeight;
    int         winWidth;
    int         winHeight;
    int         gpuVendorID;
    int         gpuDeviceID;
    int         maxTextureSize;
    bool        supportsCompute;
    bool        supports3DTextures;
};

// Struct to hold application state
struct App {
    Graphics* graphics;
    SynModelLoader* synModels;
    Phys*                    physicsManager;
    bool                     debug = true;
    std::string              appName;
};

// Core class to manage the application
class Core {
  public:
    // Pointer to the application state
    App* app;

    Core(std::string appName = "SyngineGame");
    ~Core();

    // Attach the graphics system to the core.
    // Returns 0 on success, non-zero on failure.
    int AttachGraphics(Graphics* graphics);

    // Detach the graphics system from the core.
    // Returns 0 on success, non-zero on failure.
    int DetachGraphics();

    // Main event loop, blocks until the application is closed.
    // Returns 0 on clean exit, non-zero on error.
    int SyngineEventLoop();

    // Add a new gameobject to the scene
    // Returns 0 on success, non-zero on failure
    int CreateGameobject(GameObject* gameobject);

    // Delete a gameobject from the scene and free its resources
    // Returns 0 on success, non-zero on failure
    int DeleteGameobject(GameObject* gameobject);

    // Get system specifications
    Syngine::HardwareSpecs GetSystemSpecifications();
};

} // namespace Syngine