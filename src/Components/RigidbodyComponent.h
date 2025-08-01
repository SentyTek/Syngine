#pragma once
#include "Components.h"
#include "SynginePhys.h"
#include "../src/Components/TransformComponent.h"

namespace Syngine {
// Forward declaration of TransformComponent
class TransformComponent;

// Enum for different physics shapes supported by the RigidbodyComponent.
// These shapes are used to define the physical properties of the rigidbody.
enum class PhysicsShapes {
    SPHERE,
    BOX,
    CAPSULE,
    CAPULE_TAPERED,
    CYLINDER,
    CYLINDER_TAPERED,
    CONE,
    CONVEX_HULL,
    PLANE,
    MESH,
};

struct RigidbodyParameters {
    PhysicsShapes shape = PhysicsShapes::BOX; // The shape of the rigidbody
    float         mass = 0.0f;  // Mass of the rigidbody. If 0 (which it is by default), Jolt will calculate it based on the shape.
    float         friction = 0.5f; // Friction coefficient
    float         restitution = 0.5f; // Restitution coefficient (bounciness)
    std::vector<float> shapeParameters = { 1.0f, 1.0f, 1.0f }; // Additional parameters for the shape, e.g., radius for sphere, half extents for box
    JPH::EMotionType motionType = JPH::EMotionType::Dynamic; // Motion type of the rigidbody
    JPH::ObjectLayer layer = Syngine::Layers::MOVING; // Layer of the rigidbody
};

/*
    Syngine Physics Component
    The RigidbodyComponent is used to represent a physics body in the game
   world. It holds the BodyID and the shape of the physics body from Jolt, among
   other properties.
*/
class RigidbodyComponent : public Syngine::Component {
  public:
    static constexpr Syngine::Components componentType = SYN_COMPONENT_RIGIDBODY;

    RigidbodyComponent(GameObject* owner, Syngine::RigidbodyParameters params = {});
    ~RigidbodyComponent();

    Syngine::Components getComponentType() override;

    void Init(Syngine::RigidbodyParameters params);
    void Update(bool simulate);
    void Destroy();
    
    JPH::BodyID           GetBodyID() const;
    Syngine::Phys* GetPhysicsManager() const;
    float                 GetMass() const;
    float                 GetFriction() const;
    float                 GetRestitution() const;

    std::vector<float> GetShapeParameters() const;

    void UpdateShapeParameters(const std::vector<float>& newShapeParameters);
    void SetFriction(float newFriction);
    void SetRestitution(float newRestitution);

  private:
    TransformComponent*   transform; // Reference to the transform component
    Syngine::Phys* physicsManager; // Reference to the physics manager
    JPH::BodyID           bodyID;         // ID of the physics body
    PhysicsShapes         shape;          // Shape of the physics body
    float                 mass = 0.0f;    // Mass of the physics body
    float                 friction = 0.5f; // Friction of the physics body
    float restitution = 0.5f; // Restitution of the physics body, default to 0.5
    std::vector<float>

        shapeParameters; // Parameters for the shape, e.g., radius for sphere,
                         // half extents for box
};
} // namespace Syngine