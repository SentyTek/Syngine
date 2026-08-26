// ╒════════════════ imgui_impl_bgfx.hpp ═╕
// │ Syngine                              │
// │ Created 2026-08-22                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Licensed under the MIT License       │
// ╰──────────────────────────────────────╯

// Dear ImGui: Renderer Backend for bgfx
// This needs to be used along with a Platform Backend (e.g. SDL3, GLFW, Win32,
// etc.) (Info: bgfx is a cross-platform rendering library)

// Unfortunately, bgfx does not provide a coherent imgui backend despite using
// it in its examples and vendoring it. This file aims to provide a more
// consistent and usable backend for integrating Dear ImGui with bgfx.

#pragma once

#include <Syngine/Graphics/Rendering/Renderer.h>

#include <bgfx/bgfx.h>
#include "../imgui.h"
#include "Syngine/Graphics/Resources/ShaderManager.h"

#ifndef IMGUI_DISABLE

namespace Syngine::UI::Debug {

class ImGui_ImplBgfx {
    Syngine::Shader*    m_shader = nullptr;
    bgfx::VertexLayout  m_layout;
    bgfx::TextureHandle m_font         = BGFX_INVALID_HANDLE;
    ImGuiContext*       m_imguiContext = nullptr;

  public:
    void Init();
    void Shutdown();

    void NewFrame();
    void Render(ViewID viewID);
    void EndFrame();

    void HandleEvent(SDL_Event* event);
    bool WantCaptureMouse();
    bool WantCaptureKeyboard();

    [[nodiscard("Converts a bgfx::TextureHandle to an "
                "ImTextureID")]] static inline ImTextureID
    ToImGui(bgfx::TextureHandle texture) {
        return static_cast<ImTextureID>(texture.idx);
    };

    [[nodiscard("Converts an ImTextureID to a "
                "bgfx::TextureHandle")]] static inline bgfx::TextureHandle
    ToBGFX(ImTextureID texture) {
        return bgfx::TextureHandle{ static_cast<uint16_t>(texture) };
    };
};

using SImGui = ImGui_ImplBgfx;

} // namespace Syngine::UI::Debug

#endif // #ifndef IMGUI_DISABLE
