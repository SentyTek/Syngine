// ╒═══════════════════ GizmoOverlay.cpp ═╕
// │ Syngine                              │
// │ Created 2026-10-10                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Licensed under the MIT License       │
// ╰──────────────────────────────────────╯

#ifdef SYN_IS_EDITOR

#include <Syngine/Core/Logger.h>
#include <Syngine/GameObjects/Components/MeshComponent.h>
#include <Syngine/Graphics/Rendering/GizmoOverlay.h>

#include <bgfx/bgfx.h>

#include <memory>
#include <unordered_map>
#include <unordered_set>

namespace Syngine::Debug {

namespace {
std::string                                                     s_bundle;
std::vector<GizmoOverlay::Item>                                 s_items;
std::unordered_map<std::string, std::unique_ptr<MeshComponent>> s_meshes;
std::unordered_set<std::string>                                 s_failed;
bgfx::UniformHandle s_colorUniform = BGFX_INVALID_HANDLE;

MeshComponent* GetMesh(const std::string& name) {
    auto it = s_meshes.find(name);
    if (it != s_meshes.end()) return it->second.get();
    if (s_failed.contains(name) || s_bundle.empty()) return nullptr;

    auto mesh = std::make_unique<MeshComponent>(nullptr);
    mesh->bakeNodeTransforms =
        true; // gizmo parts are oriented via node transforms
    if (!mesh->LoadMesh(s_bundle, name + ".glb", false)) {
        s_failed.insert(name);
        return nullptr;
    }
    MeshComponent* raw = mesh.get();
    s_meshes.emplace(name, std::move(mesh));
    return raw;
}
} // namespace

void GizmoOverlay::SetMeshBundle(const std::string& bundlePath) {
    s_bundle = bundlePath;
}

void GizmoOverlay::Preload(const std::vector<std::string>& meshNames) {
    for (const std::string& name : meshNames) {
        MeshComponent* mesh = GetMesh(name);
    }
}

void GizmoOverlay::Submit(const Item& item) { s_items.push_back(item); }

void GizmoOverlay::_Draw(uint16_t viewId, uint64_t state, uint16_t program) {
    if (!bgfx::isValid(s_colorUniform)) {
        s_colorUniform =
            bgfx::createUniform("u_gizmoColor", bgfx::UniformType::Vec4);
    }

    // Finish any pending async mesh loads
    for (auto& [name, mesh] : s_meshes) mesh->Update(0.0f);

    for (const Item& item : s_items) {
        MeshComponent* mesh = GetMesh(item.meshName);
        if (!mesh || !mesh->IsMeshLoaded()) continue;

        bgfx::setTransform(item.model.data());
        bgfx::setUniform(s_colorUniform, item.color.data());
        bgfx::setVertexBuffer(0, mesh->modelData.vbh);
        bgfx::setIndexBuffer(mesh->modelData.ibh);
        bgfx::setState(state);
        bgfx::submit(viewId, bgfx::ProgramHandle{ program });
    }
}

void GizmoOverlay::_EndFrame() { s_items.clear(); }

void GizmoOverlay::_Shutdown() {
    s_items.clear();
    s_meshes.clear();
    s_failed.clear();
    if (bgfx::isValid(s_colorUniform)) {
        bgfx::destroy(s_colorUniform);
        s_colorUniform = BGFX_INVALID_HANDLE;
    }
}

} // namespace Syngine::Debug

#endif // SYN_IS_EDITOR
