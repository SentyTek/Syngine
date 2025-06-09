#pragma once
#include "Components.h"
#include "SynginePhys.h"
#include "TransformComponent.h"

namespace Syngine {
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

/*
    Syngine Physics Component
    The RigidbodyComponent is used to represent a physics body in the game
   world. It holds the BodyID and the shape of the physics body from Jolt, among
   other properties.
*/
class RigidbodyComponent : public SynComponent {
  public:
    static constexpr SynComponents componentType = SYN_COMPONENT_RIGIDBODY;

    RigidbodyComponent(GameObject* owner);
    ~RigidbodyComponent();

    SynComponents getComponentType() override;

    void Init(TransformComponent*       transform,
              Syngine::SynginePhys*     physicsManager,
              PhysicsShapes             shape,
              const std::vector<float>& parameters,
              JPH::EMotionType          motionType,
              JPH::ObjectLayer          layer,
              const std::vector<float>& shapeParameters);
    void Update(bool simulate);
    void Destroy();
    
    JPH::BodyID           GetBodyID() const;
    Syngine::SynginePhys* GetPhysicsManager() const;
    float                 GetMass() const;
    float                 GetFriction() const;
    float                 GetRestitution() const;

    std::vector<float> GetShapeParameters() const;

    void UpdateShapeParameters(const std::vector<float>& newShapeParameters);
    void SetFriction(float newFriction);
    void SetRestitution(float newRestitution);

  private:
    TransformComponent*   transform; // Reference to the transform component
    Syngine::SynginePhys* physicsManager; // Reference to the physics manager
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