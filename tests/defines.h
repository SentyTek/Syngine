// ╒══════════════════════════ defines.h ═╕
// │ Syngine                              │
// │ Created 2026-06-13                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// | Licensed under the MIT License       |
// ╰──────────────────────────────────────╯

#define FLOAT_MARGIN 0.001f

#include <Syngine/Syngine.h>
#include <string>
using namespace Syngine;

#define SYN_STARTENGINE                                                        \
    std::string  gameName = "TestGame";                                        \
    EngineConfig config   = {                                                  \
          .gameName = gameName,                                                \
          .headless = true,                                                    \
    };                                                                         \
    Core engine(config);                                                       \
    engine.Initialize(RendererConfig{});                                       \

[[nodiscard]] inline GameObject* CreateRigidbodyObject(float mass = 1.0f) {
    GameObject* go = new GameObject("TestObject", "default");
    go->AddComponent<TransformComponent>();
    std::vector<float> boxExtents = { 1.0f, 1.0f, 1.0f }; // half extents for box shape
    RigidbodyParameters params = {
        .shape           = PhysicsShapes::BOX,
        .mass            = mass,
        .friction        = 0.5f,
        .restitution     = 0.1f,
        .shapeParameters = boxExtents,
        .motionType      = JPH::EMotionType::Dynamic,
        .layer           = Layers::MOVING
    };
    go->AddComponent<RigidbodyComponent>(params);
    return go;
}
