// ╒═════════════ BillboardComponent.cpp ═╕
// │ Syngine                              │
// │ Created 2025-12-16                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// | Licensed under the MIT License       |
// ╰──────────────────────────────────────╯

#include "Syngine/ECS/Components/BillboardComponent.h"
#include "Syngine/ECS/ComponentRegistry.h"
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
    this->m_texturePath = texturePath;
    this->m_bundlePath = "imgs/imgs.spk";
    this->Init("imgs/imgs.spk", texturePath);
}

BillboardComponent::BillboardComponent(GameObject* owner,
                                       std::string bundlePath,
                                       std::string texturePath,
                                       BillboardMode mode,
                                       float size) {
    this->m_owner = owner;
    this->size    = size;
    this->m_mode  = mode;
    this->m_texturePath = texturePath;
    this->m_bundlePath = bundlePath;
    this->Init(bundlePath, texturePath);
}

BillboardComponent::BillboardComponent(const BillboardComponent& other) {
    this->m_owner = other.m_owner;
    this->size    = other.size;
    this->m_mode  = other.m_mode;
    this->m_texturePath = other.m_texturePath;
    this->m_bundlePath = other.m_bundlePath;
    this->m_texture = BGFX_INVALID_HANDLE;
    if (!this->m_texturePath.empty()) {
        this->Init(this->m_bundlePath, this->m_texturePath);
    }
}

BillboardComponent& BillboardComponent::operator=(const BillboardComponent& other) {
    if (this != &other) {
        if (bgfx::isValid(this->m_texture)) {
            bgfx::destroy(this->m_texture);
            this->m_texture = BGFX_INVALID_HANDLE;
        }
        this->m_owner = other.m_owner;
        this->size    = other.size;
        this->m_mode  = other.m_mode;
        this->m_texturePath = other.m_texturePath;
        this->m_bundlePath = other.m_bundlePath;
        if (!this->m_texturePath.empty()) {
            this->Init(this->m_bundlePath, this->m_texturePath);
        }
    }
    return *this;
}

BillboardComponent::~BillboardComponent() {
    if (bgfx::isValid(this->m_texture)) {
        bgfx::destroy(this->m_texture);
        this->m_texture = BGFX_INVALID_HANDLE;
    }
}

Syngine::ComponentTypeID BillboardComponent::GetComponentType() {
    return SYN_COMPONENT_BILLBOARD;
}

Serializer::DataNode BillboardComponent::Serialize() const {
    Serializer::DataNode billboardNode;
    billboardNode / "type" = static_cast<int>(SYN_COMPONENT_BILLBOARD);
    billboardNode / "size" = size;
    billboardNode / "mode" = static_cast<int>(m_mode);
    billboardNode / "texturePath" = m_texturePath;
    billboardNode / "bundlePath" = m_bundlePath;
        // Note: We don't serialize the texture handle itself, as it's not
        // meaningful outside of the current runtime context. Instead, we serialize
        // the texture path, which can be used to reload the texture when deserializing.
    return billboardNode;
}

void BillboardComponent::Init(const std::string& bundlePath, const std::string& texturePath) {
    // Load the texture for the billboard
    std::string resolvedBundlePath =
        Syngine::Internal::ResolvePath(bundlePath.c_str());

    this->m_texture = Syngine::LoadTextureFromBundle(
        resolvedBundlePath, texturePath);

    if (!bgfx::isValid(this->m_texture)) {
        Syngine::Logger::LogF(Syngine::LogLevel::ERR, true,
                              "Failed to load billboard texture from %s",
                              texturePath.c_str());
    }
}

static Syngine::ComponentRegistrar s_billboardRegistrar(
    Syngine::SYN_COMPONENT_BILLBOARD,
    // ParseXML: XML element -> DataNode
    [](const scl::xml::XmlElem* elem) -> Serializer::DataNode {
        Serializer::DataNode node;
        node / "type" = static_cast<Syngine::ComponentTypeID>(SYN_COMPONENT_BILLBOARD);
        for (const auto& attr : elem->attributes()) {
            scl::string key = attr->tag();
            scl::string value = attr->data();
            if (key == "size") {
                node / "size" = std::stof(value.cstr());
            } else if (key == "mode") {
                node / "mode" = std::stoi(value.cstr());
            } else if (key == "texturePath") {
                node / "texturePath" = value;
            } else if (key == "bundlePath") {
                node / "bundlePath" = value;
            }
        }
        return node;
    },

    // Instantiate: DataNode -> Component instance
    [](GameObject* owner, const Serializer::DataNode& data) -> std::unique_ptr<Component> {
        float size = data.Has("size") ? data["size"].As<float>() : 1.0f;
        BillboardMode mode = BillboardMode::CAMERA_ALIGNED;
        if (data.Has("mode")) {
            int modeInt = data["mode"].As<int>();
            if (modeInt >= 0 && modeInt < static_cast<int>(BillboardMode::COUNT)) {
                mode = static_cast<BillboardMode>(modeInt);
            }
        }
        std::string texturePath = data.Has("texturePath") ? data["texturePath"].As<std::string>() : "";
        std::string bundlePath = data.Has("bundlePath") ? data["bundlePath"].As<std::string>() : "";
        return std::make_unique<BillboardComponent>(owner, bundlePath, texturePath, mode, size);
    }
);

} // namespace Syngine
