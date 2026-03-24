// ╒═══════════════ BillboardComponent.h ═╕
// │ Syngine                              │
// │ Created 2025-12-16                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#pragma once
#include "Syngine/ECS/Component.h"
#include "Syngine/ECS/GameObject.h"

#include <bgfx/bgfx.h>

#include <string>

namespace Syngine {

/// @brief Enum that describes how a billboard should render
/// @section BillboardComponent
/// @since v0.0.1
enum class BillboardMode {
    CAMERA_ALIGNED = 0, //* Always faces the camera
    AXIS_Y_ALIGNED = 1, //* Rotates around Y (vertical) axis only
    FIXED          = 2, //* No rotation, fixed orientation
    COUNT          = 3  //* Number of billboard modes (for validation)
};


/// @brief Syngine Billboard Component. The BillboardComponent is used to
/// represent a 2D model in the 3D game world.
/// @section BillboardComponent

class BillboardComponent : public Syngine::Component {
    
    BillboardMode m_mode = BillboardMode::CAMERA_ALIGNED; //* Billboard rendering mode

    bgfx::TextureHandle m_texture =
        BGFX_INVALID_HANDLE; //* Texture handle for the billboard
    std::string m_texturePath; //* Path to the billboard texture

    GameObject* m_owner; // Reference to the owner game object

    float m_rot[3] = { 0.0f, 0.0f, 0.0f }; //* Rotation of the billboard
  public:
    
    float size = 1.0f; //* Size of the billboard
    bool  receiveShadows = true; //* Whether the billboard receives shadows
    bool  receiveSunLight = true; //* Whether the billboard receives sunlight
    
    static constexpr Syngine::Components componentType =
        SYN_COMPONENT_BILLBOARD; //* Billboard component type

    /// @brief Construct a new Billboard Component object
    /// @param owner Pointer to the owner GameObject
    /// @param texturePath Path to the billboard texture
    /// @param mode Billboard rendering mode
    /// @param size Size of the billboard
    /// @since v0.0.1
    /// @note The texturePath should be a valid path to a texture file, relative
    /// to the game's assets directory. owner GO handled by the ECS.
    BillboardComponent(GameObject*   owner,
                       std::string   texturePath,
                       BillboardMode mode = BillboardMode::CAMERA_ALIGNED,
                       float         size = 1.0f);
    
    BillboardComponent(const BillboardComponent& other);
    BillboardComponent& operator=(const BillboardComponent& other);

    ~BillboardComponent();

    /// @brief Get the type of this component
    /// @return The component type as an enum value
    Components GetComponentType() override;
    std::unique_ptr<Component> Clone() const override {
        return std::make_unique<BillboardComponent>(*this);
    }

    /// @brief Serializes the BillboardComponent to a data node
    /// @return A pointer to the serialized data node representing the
    /// BillboardComponent's state
    Serializer::DataNode Serialize() const override;

    /// @brief Initialize the billboard component
    /// @param texturePath Path to the billboard texture
    /// @note This should only be called when the component is added to a
    /// GameObject
    /// @since v0.0.1
    /// @internal
    void Init(const std::string& texturePath);

    /// @brief Update the billboard component
    /// @since v0.0.1
    void Update() {};

    /// @brief Get the texture handle of the billboard
    /// @return bgfx::TextureHandle The texture handle
    /// @threadsafety read-only
    /// @since v0.0.1
    /// @internal
    bgfx::TextureHandle _GetTexture() const { return this->m_texture; }

    /// @brief Set rotation around X, Y, Z axes
    /// @param rot Rotation around X axis in radians
    void SetRotX(float rotX) { this->m_rot[0] = rotX; }

    /// @brief Set rotation around X, Y, Z axes
    /// @param rot Rotation around X axis in radians
    void SetRotY(float rotY) { this->m_rot[1] = rotY; }

    /// @brief Set rotation around X, Y, Z axes
    /// @param rot Rotation around X axis in radians
    void SetRotZ(float rotZ) { this->m_rot[2] = rotZ; }

    /// @brief Set rotation around X, Y, Z axes
    /// @param rotX Rotation around X axis in radians
    /// @param rotY Rotation around Y axis in radians
    /// @param rotZ Rotation around Z axis in radians
    void SetRot(float rotX, float rotY, float rotZ) {
        this->m_rot[0] = rotX;
        this->m_rot[1] = rotY;
        this->m_rot[2] = rotZ;
    }

    /// @brief Get rotation around X, Y, Z axes
    /// @return float* Pointer to array of 3 floats representing rotation
    float* GetRot() { return this->m_rot; }

    /// @brief Get the billboard mode
    /// @return BillboardMode The billboard rendering mode
    BillboardMode GetMode() const { return this->m_mode; }
};

} // namespace Syngine