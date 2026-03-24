// ╒═══════════ ═══ RigidbodyComponent.h ═╕
// │ Syngine                              │
// │ Created 2025-05-22                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#pragma once
#include "Syngine/ECS/Component.h"
#include "Syngine/Physics/Physics.h"

namespace Syngine {
// Forward declaration of TransformComponent
class TransformComponent;

/// @brief Enum for different physics shapes supported by the
/// RigidbodyComponent. These shapes are used to define the physical properties
/// of the rigidbody.
/// @section RigidbodyComponent
/// @since v0.0.1
enum class PhysicsShapes {
    SPHERE,           //* Sphere shape
    BOX,              //* Box shape
    CAPSULE,          //* Capsule shape
    CAPSULE_TAPERED,  //* Tapered capsule shape
    CYLINDER,         //* Cylinder shape
    CYLINDER_TAPERED, //* Tapered cylinder shape
    CONE,             //* Cone shape
    CONVEX_HULL,      //* Convex hull shape
    PLANE,            //* Plane shape
    MESH,             //* Mesh shape
    COMPOUND,         //* Compound shape
};

/// @brief Enum for different force application modes
/// @section RigidbodyComponent
/// @since v0.0.1
enum class ForceMode {
    FORCE, //* Add a continuous force to the rigidbody using its mass (F = m *
           //* a) (Newtons)
    ACCELERATION, //* Add a continuous acceleration to the rigidbody, ignoring
                  //* its mass. Note: This is not well supported, and is
                  //* equivalent to FORCE if you did NOT manually set the mass
                  //* yourself.
    IMPULSE, //* Add an instant force impulse, using its mass (I = m * dv) (
             //* Newton-seconds)
    VELOCITY_CHANGE //* Change velocity instantaneously, ignoring its mass.
                    //Note:
                    //* This is not well supported, and is equivalent to IMPULSE
                    //if
                    //* you did NOT manually set the mass yourself.
};

struct CompoundShapePart {
    PhysicsShapes shape;
    std::vector<float> shapeParameters;
    float              position[3]; // Local position offset
    float              rotation[4]; // Local rotation quaternion
};

/// @brief Struct to hold parameters for the RigidbodyComponent.
/// This struct is used to initialize the RigidbodyComponent with specific
/// properties.
/// @section RigidbodyComponent
/// @since v0.0.1
struct RigidbodyParameters {
    PhysicsShapes shape = PhysicsShapes::BOX; // The shape of the rigidbody
    float         mass = 0.0f;  // Mass of the rigidbody. If 0 (which it is by default), Jolt will calculate it based on the shape.
    float         friction = 0.5f; // Friction coefficient
    float         restitution = 0.5f; // Restitution coefficient (bounciness)
    std::vector<float> shapeParameters = { 1.0f, 1.0f, 1.0f }; // Additional parameters for the shape, e.g., radius for sphere, half extents for box
    JPH::EMotionType motionType = JPH::EMotionType::Dynamic; // Motion type of the rigidbody
    JPH::ObjectLayer layer = Syngine::Layers::MOVING; // Layer of the rigidbody
    std::vector<CompoundShapePart> compoundParts = {}; // Parts for compound shape
};

/*
 * @brief Syngine Rigidbody Component The RigidbodyComponent is used to
 * represent a physics body in the game world. It holds the BodyID and the shape of
 * the physics body from Jolt, among other properties.
 * @section RigidbodyComponent
 * @since v0.0.1
 */
class RigidbodyComponent : public Syngine::Component {
  public:
    static constexpr Syngine::ComponentTypeID componentType = SYN_COMPONENT_RIGIDBODY; //* Rigidbody component type

    /// @brief Constructor for the RigidbodyComponent class
    /// @param owner Pointer to the GameObject that owns this component
    /// @param params Rigidbody parameters to initialize the component
    /// @note This should only be called by GameObject::AddComponent<T>()
    /// @since v0.0.1
    RigidbodyComponent(GameObject*                  owner,
                       Syngine::RigidbodyParameters params = {});

    RigidbodyComponent(const RigidbodyComponent& other);
    RigidbodyComponent& operator=(const RigidbodyComponent& other);
    
    ~RigidbodyComponent();

    /// @brief Get the type of this component
    /// @return The component type as an enum value
    /// @threadsafety read-only
    /// @since v0.0.1
    Syngine::ComponentTypeID GetComponentType() override;

    /// @brief Clone the RigidbodyComponent
    /// @return A unique pointer to the cloned RigidbodyComponent
    std::unique_ptr<Component> Clone() const override {
        return std::make_unique<RigidbodyComponent>(*this);
    }

    /// @brief Serializes the RigidbodyComponent to a data node
    /// @return A pointer to the serialized data node representing the
    /// RigidbodyComponent's state
    Serializer::DataNode Serialize() const override;

    /// @brief Initialize the RigidbodyComponent
    /// @param params Rigidbody parameters to initialize the component
    /// @note This should only be called when the component is added to a GameObject
    /// @threadsafety not-safe
    /// @since v0.0.1
    void Init(Syngine::RigidbodyParameters params);

    /// @brief Update the RigidbodyComponent
    /// @param simulate Whether to simulate physics or not
    /// @note This is called every frame to update the physics body
    /// @threadsafety not-safe
    /// @since v0.0.1
    void Update(bool simulate);

    /// @brief Destroys the RigidbodyComponent and its associated physics body
    /// @note This is called when the component is removed or the GameObject is
    /// destroyed
    /// @threadsafety not-safe
    /// @since v0.0.1
    void Destroy(); //TODO: Move this to RAII in the destructor

    /// @brief Get the BodyID of the physics body
    /// @return The ID of the physics body
    /// @threadsafety read-only
    /// @since v0.0.1
    /// @internal
    JPH::BodyID _GetBodyID() const;

    /// @brief Gets the physics manager
    /// @return Pointer to the physics manager
    /// @threadsafety read-only
    /// @since v0.0.1
    /// @internal
    Syngine::Phys* _GetPhysicsManager() const;

    /// @brief Get the mass of the physics body.
    /// @return The mass of the physics body
    /// @note If this value is 0, Jolt has used the shape to calculate the mass,
    /// and therefore this value is not valid. It is not recommended to use
    /// this for anything really.
    /// @threadsafety read-only
    /// @since v0.0.1
    float GetMass() const;

    /// @brief Get the friction of the physics body.
    /// @return The friction of the physics body
    /// @threadsafety read-only
    /// @since v0.0.1
    float GetFriction() const;

    /// @brief Get the restitution of the physics body.
    /// @return The restitution of the physics body
    /// @threadsafety read-only
    /// @since v0.0.1
    float GetRestitution() const;

    /// @brief Get the shape parameters of the physics body
    /// @return The shape parameters of the physics body
    /// @note This is a vector of floats that can be used to define the shape
    /// parameters, e.g., radius for sphere, half extents for box, etc.
    /// @threadsafety read-only
    /// @since v0.0.1
    std::vector<float> GetShapeParameters() const;

    /// @brief Set the shape parameters of the physics body
    /// @param newShapeParameters The new shape parameters to set
    /// @threadsafety not-safe
    /// @since v0.0.1
    void UpdateShapeParameters(const std::vector<float>& newShapeParameters);

    /// @brief Set the friction of the physics body
    /// @param newFriction The new friction value to set
    /// @threadsafety not-safe
    /// @since v0.0.1
    void SetFriction(float newFriction);
    
    /// @brief Set the restitution of the physics body
    /// @param newRestitution The new restitution value to set
    /// @threadsafety not-safe
    /// @since v0.0.1
    void SetRestitution(float newRestitution);

    /// @brief Add a force to the rigidbody
    /// @param force The force to add (vec3)
    /// @param mode The mode of force application
    /// @note Force values for visible movement may be higher than expected.
    /// @threadsafety not-safe
    /// @since v0.0.1
    void AddForce(const float* force, ForceMode mode = ForceMode::FORCE);

    /// @brief Add a force to the rigidbody at a specific position
    /// @param force The force to add (vec3)
    /// @param position The position to apply the force at (vec3)
    /// @param mode The mode of force application
    /// @note Force values for visible movement may be higher than expected.
    /// @threadsafety not-safe
    /// @since v0.0.1
    void AddForceAtPosition(const float* force,
                            const float* position,
                            ForceMode    mode = ForceMode::FORCE);

    /// @brief Add a torque to the rigidbody
    /// @param torque The torque to add (vec3)
    /// @param mode The mode of torque application
    /// @note Torque values for visible rotation may be higher than expected.
    /// @threadsafety not-safe
    /// @since v0.0.1
    void AddTorque(const float* torque, ForceMode mode = ForceMode::FORCE);

  private:
    TransformComponent*   transform; // Reference to the transform component
    Syngine::Phys* physicsManager; // Reference to the physics manager
    JPH::BodyID           bodyID;         // ID of the physics body
    PhysicsShapes         shape;          // Shape of the physics body
    float                 mass = 0.0f;    // Mass of the physics body
    float                 friction = 0.5f; // Friction of the physics body
    float restitution = 0.5f; // Restitution of the physics body, default to 0.5
    std::vector<float> shapeParameters; // Parameters for the shape, e.g., radius for sphere,
                         // half extents for box

    void _MatrixToQuat(float* outQuat, const float* mtx);
};
} // namespace Syngine