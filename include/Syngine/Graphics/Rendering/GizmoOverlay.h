// ╒════════════════════ GizmoOverlay.h ═╕
// │ Syngine                              │
// │ Created 2026-05-14                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Licensed under the MIT License       │
// ╰──────────────────────────────────────╯

#ifdef SYN_IS_EDITOR

#pragma once
#include <Syngine/Math/Math.hpp>

#include <string>
#include <vector>

namespace Syngine::Debug {

/// @brief Immediate-mode list of colored meshes drawn in VIEW_GIZMO. They are
/// drawn over scene geometry and physics wireframes, but below billboard
/// gizmos. Items are cleared at the end of every rendered frame, so they must
/// be submitted every frame.
/// @details The pass uses whichever shader program is loaded for VIEW_GIZMO
/// (see ShaderManager::LoadShader). That program should read a_position and
/// a_normal, and may declare `uniform vec4 u_gizmoColor;` which is set per
/// item.
/// @since v0.0.2
class GizmoOverlay {
  public:
    struct Item {
        std::string meshName; //* Mesh name within the bundle
        Math::Mat4  model;    //* World matrix
        Math::Vec4  color;    //* RGBA, passed as u_gizmoColor
    };

    /// @brief Set the bundle gizmo meshes are loaded from
    /// @param bundlePath Bundle path (e.g. "meshes/meshes.spk")
    static void SetMeshBundle(const std::string& bundlePath);

    /// @brief Synchronously load meshes from the bundle set by
    /// SetMeshBundle, one at a time. Meant to be called during startup
    /// asset import. Meshes that fail to load are logged and skipped.
    /// @param meshNames Mesh names within the bundle (no extension)
    static void Preload(const std::vector<std::string>& meshNames);

    /// @brief Queue an item for the next rendered frame
    static void Submit(const Item& item);

    /// @internal
    static void _Draw(uint16_t viewId, uint64_t state, uint16_t program);
    /// @internal
    static void _EndFrame();
    /// @internal
    static void _Shutdown();
};

} // namespace Syngine::Debug

#endif
