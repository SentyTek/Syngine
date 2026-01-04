// ╒═════════════ BillboardComponent.cpp ═╕
// │ Syngine                              │
// │ Created 2025-12-16                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#include "Syngine/ECS/Components/BillboardComponent.h"
#include "Syngine/ECS/GameObject.h"
#include "Syngine/Utils/FsUtils.h"
#include "Syngine/Graphics/TextureHelpers.h"

namespace Syngine {
BillboardComponent::BillboardComponent(GameObject* owner, 
                                       std::string texturePath,
                                       BillboardMode mode,
                                       float size) {
    this->m_owner = owner;
    this->size    = size;
    this->m_mode  = mode;
    this->Init(texturePath);
}

BillboardComponent::BillboardComponent(const BillboardComponent& other) {
    this->m_owner = other.m_owner;
    this->size    = other.size;
    this->m_mode  = other.m_mode;
    this->m_texture = other.m_texture;
}

BillboardComponent& BillboardComponent::operator=(const BillboardComponent& other) {
    if (this != &other) {
        this->m_owner = other.m_owner;
        this->size    = other.size;
        this->m_mode  = other.m_mode;
        this->m_texture = other.m_texture;
    }
    return *this;
}

BillboardComponent::~BillboardComponent() {
    // No specific cleanup needed for now
}

Syngine::Components BillboardComponent::GetComponentType() {
    return SYN_COMPONENT_BILLBOARD;
}

void BillboardComponent::Init(const std::string& texturePath) {
    // Load the texture for the billboard
    this->m_texture = Syngine::LoadTextureFromFile(
        Syngine::_ResolveOSPath(texturePath.c_str()).c_str());

    if (!bgfx::isValid(this->m_texture)) {
        Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                              "Failed to load billboard texture from %s",
                              texturePath.c_str());
    }
}

} // namespace Syngine