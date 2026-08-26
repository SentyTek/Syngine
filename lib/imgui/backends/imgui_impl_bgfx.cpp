// ╒════════════════ imgui_impl_bgfx.cpp ═╕
// │ Syngine                              │
// │ Created 2026-08-22                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Licensed under the MIT License       │
// ╰──────────────────────────────────────╯

#include "imgui_impl_bgfx.hpp"
#include "Syngine/Core/Logger.h"
#include "Syngine/Graphics/Rendering/Renderer.h"
#include "bgfx/bgfx.h"

#include "bgfx/defines.h"
#include "imgui/backends/imgui_impl_sdl3.h"
#include "imgui/imgui.h"

#include <Syngine/Graphics/Rendering/RenderDirector.h>
#include <Syngine/Graphics/Resources/ShaderManager.h>
#include <Syngine/Graphics/Windowing.h>

#include <bx/math.h>

namespace Syngine::UI::Debug {

void ImGui_ImplBgfx::Init() {
    IMGUI_CHECKVERSION();
    m_imguiContext = ImGui::CreateContext();

    // yay imgui
    ImGuiIO& io = ::ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |=
        ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigWindowsMoveFromTitleBarOnly = true;

    ImGui_ImplSDL3_InitForOther(Syngine::Window::_GetSDLWindow());

    unsigned char* pixels;
    int            width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

    // bgfx stuff
    ShaderManager::LoadShader("shaders/assets_root.spk", "ui", ViewID::VIEW_UI);
    m_layout.begin()
        .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
        .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
        .end();

    m_shader = ShaderManager::Get("ui");

    m_font = bgfx::createTexture2D(static_cast<uint16_t>(width),
                                   static_cast<uint16_t>(height),
                                   false,
                                   1,
                                   bgfx::TextureFormat::RGBA8,
                                   BGFX_TEXTURE_NONE,
                                   bgfx::copy(pixels, width * height * 4));
    io.Fonts->SetTexID(ToImGui(m_font));
}

void ImGui_ImplBgfx::Shutdown() {
    if (bgfx::isValid(m_font)) {
        bgfx::destroy(m_font);
        m_font = BGFX_INVALID_HANDLE;
    }
    ShaderManager::UnloadShader(m_shader->id);
    m_shader = nullptr;
    if (m_imguiContext) {
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext(m_imguiContext);
        m_imguiContext = nullptr;
    }
}

void ImGui_ImplBgfx::NewFrame() {
    // Actually a pretty boring function since the renderer manages the bgfx
    // view data
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
}

void ImGui_ImplBgfx::EndFrame() { ImGui::EndFrame(); }

void ImGui_ImplBgfx::Render(ViewID viewID) {
    // Actually not a boring function (interrobang)
    static_assert(sizeof(ImDrawIdx) == 2);
    ImGui::Render();

    ImDrawData* drawData = ImGui::GetDrawData();

    // bgfx errors when rendering 0 vertices, so we early out here.
    if (!drawData || drawData->TotalVtxCount == 0) {
        return;
    }

    float viewProjection[16];
    bx::mtxOrtho(viewProjection,
                 drawData->DisplayPos.x,
                 drawData->DisplayPos.x + drawData->DisplaySize.x,
                 drawData->DisplayPos.y + drawData->DisplaySize.y,
                 drawData->DisplayPos.y,
                 0.0f,
                 1000.0f,
                 0.0f,
                 bgfx::getCaps()->homogeneousDepth);
    bgfx::setViewTransform(viewID, nullptr, viewProjection);

    // if there are not enough transient buffers available, we early out
    if (bgfx::getAvailTransientVertexBuffer(drawData->TotalVtxCount, m_layout) <
            drawData->TotalVtxCount ||
        bgfx::getAvailTransientIndexBuffer(drawData->TotalIdxCount,
                                           sizeof(ImDrawIdx) == 4) <
            drawData->TotalIdxCount) {
        return;
    }

    bgfx::TransientVertexBuffer vb;
    bgfx::TransientIndexBuffer  ib;
    bgfx::allocTransientVertexBuffer(&vb, drawData->TotalVtxCount, m_layout);
    bgfx::allocTransientIndexBuffer(
        &ib, drawData->TotalIdxCount, sizeof(ImDrawIdx) == 4);

    uint64_t state = BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A |
                     BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA,
                                           BGFX_STATE_BLEND_INV_SRC_ALPHA);

    uint32_t vertexOffset = 0;
    uint32_t indexOffset  = 0;

    for (int i = 0; i < drawData->CmdListsCount; i++) {
        const ImDrawList* cmdList = drawData->CmdLists[i];

        memcpy(vb.data + vertexOffset * sizeof(ImDrawVert),
               cmdList->VtxBuffer.Data,
               cmdList->VtxBuffer.Size * sizeof(ImDrawVert));
        memcpy(ib.data + indexOffset * sizeof(ImDrawIdx),
               cmdList->IdxBuffer.Data,
               cmdList->IdxBuffer.Size * sizeof(ImDrawIdx));

        for (const ImDrawCmd& cmd : cmdList->CmdBuffer) {
            if (cmd.UserCallback) {
                if (cmd.UserCallback == ImDrawCallback_ResetRenderState) {
                    bgfx::setState(state);
                } else {
                    cmd.UserCallback(cmdList, &cmd);
                }
                continue;
            }

            bgfx::setVertexBuffer(0,
                                  &vb,
                                  vertexOffset + cmd.VtxOffset,
                                  cmdList->VtxBuffer.Size - cmd.VtxOffset);

            bgfx::setIndexBuffer(
                &ib, indexOffset + cmd.IdxOffset, cmd.ElemCount);

            int x = static_cast<int>((cmd.ClipRect.x - drawData->DisplayPos.x) *
                                     drawData->FramebufferScale.x);

            int y = static_cast<int>((cmd.ClipRect.y - drawData->DisplayPos.y) *
                                     drawData->FramebufferScale.y);

            int w = static_cast<int>((cmd.ClipRect.z - cmd.ClipRect.x) *
                                     drawData->FramebufferScale.x);

            int h = static_cast<int>((cmd.ClipRect.w - cmd.ClipRect.y) *
                                     drawData->FramebufferScale.y);

            bgfx::setScissor(static_cast<uint16_t>(x),
                             static_cast<uint16_t>(y),
                             static_cast<uint16_t>(w),
                             static_cast<uint16_t>(h));

            bgfx::TextureHandle tex = ToBGFX(cmd.GetTexID());

            bgfx::setTexture(0, m_shader->m_textureParams[0].handle, tex);
            bgfx::setState(state);
            bgfx::submit(viewID, m_shader->GetProgram());
        }

        vertexOffset += cmdList->VtxBuffer.Size;
        indexOffset += cmdList->IdxBuffer.Size;
    }
}

void ImGui_ImplBgfx::HandleEvent(SDL_Event* event) {
    ImGui_ImplSDL3_ProcessEvent(event);
}

bool ImGui_ImplBgfx::WantCaptureMouse() {
    ImGuiIO& io = ::ImGui::GetIO();
    return io.WantCaptureMouse;
}

bool ImGui_ImplBgfx::WantCaptureKeyboard() {
    ImGuiIO& io = ::ImGui::GetIO();
    return io.WantCaptureKeyboard;
}

} // namespace Syngine::UI::Debug
