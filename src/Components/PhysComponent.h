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
    The PhysicsComponent is used to represent a physics body in the game world.
    It holds the BodyID and the shape of the physics body from Jolt, among other properties.
*/
class PhysicsComponent : public SynComponent {
    public:
    static constexpr SynComponents componentType = SYN_COMPONENT_PHYSICS;

    PhysicsComponent(GameObject* owner);
    ~PhysicsComponent();

    SynComponents getComponentType() override;

    void Init(
        TransformComponent* transform,
        Syngine::SynginePhys* physicsManager,
        PhysicsShapes shape,
        float mass,
        float friction,
        JPH::EMotionType motionType,
        JPH::ObjectLayer layer,
        const std::vector<float>& shapeParameters
    );
    void Update(bool simulate);
    void Destroy();

    private:
    TransformComponent* transform;         // Reference to the transform component
    Syngine::SynginePhys* physicsManager;  // Reference to the physics manager
    JPH::BodyID bodyID;                    // ID of the physics body
    PhysicsShapes shape;                   // Shape of the physics body
    float mass;                            // Mass of the physics body
    float friction;                        // Friction of the physics body
};
} // namespace Syngine