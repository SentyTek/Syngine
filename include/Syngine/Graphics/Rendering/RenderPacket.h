// ╒═════════════════════ RenderPacket.h ═╕
// │ Syngine                              │
// │ Created 2026-07-26                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Licensed under the MIT License       │
// ╰──────────────────────────────────────╯

#pragma once

#include <Syngine/Graphics/Resources/ShaderManager.h>
#include <Syngine/Graphics/Resources/MaterialManager.h>
#include <Syngine/Math/Matrix4x4.hpp>
#include <Syngine/GameObjects/GameObject.h>

#include <bgfx/bgfx.h>

namespace Syngine {

struct RenderPacket {
    bgfx::VertexBufferHandle vbh;
    bgfx::IndexBufferHandle  ibh;
    Math::Matrix4x4          modelMtx;
    bool                     mirror;

    uint32_t indexStart;
    uint32_t indexCount;
    uint32_t depth;

    MaterialInstance* material;
    Shader*           shader;
    GameObject*       go;

    bool visible = false;

    RenderPacket(bgfx::VertexBufferHandle vbh,
                 bgfx::IndexBufferHandle  ibh,
                 const Math::Matrix4x4&   modelMtx,
                 bool                     mirror,
                 uint32_t                 indexStart,
                 uint32_t                 indexCount,
                 uint32_t                 depth,
                 MaterialInstance*        material,
                 Shader*                  shader,
                 GameObject*              go,
                 bool                     visible)
        : vbh(vbh), ibh(ibh), modelMtx(modelMtx), mirror(mirror),
          indexStart(indexStart), indexCount(indexCount), depth(depth),
          material(material), shader(shader), go(go), visible(visible) {}

    RenderPacket() = default;
};

} // namespace Syngine
