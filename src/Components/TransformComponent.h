#pragma once
#include "Components.h"
#include "SyngineGameobject.h"

namespace Syngine {
/*
    Syngine Transform Component
    The TransformComponent is used to represent the position, rotation, and scale of a gameobject in the game world.
*/
class TransformComponent : public Syngine::Component {
    public:
        static constexpr Syngine::Components componentType = SYN_COMPONENT_TRANSFORM;

        TransformComponent(GameObject* owner);

        // Initializes the transform component with default values.
        // This is an entirely optional step, as the transform component can and is initialized
        // directly in the constructor.
        void Init(std::vector<float> position = {0.0f, 0.0f, 0.0f},
                  std::vector<float> rotation = {0.0f, 0.0f, 0.0f, 1.0f},
                  std::vector<float> scale = {1.0f, 1.0f, 1.0f});

        void Update() {}; // No update logic needed for transform component
        
        float position[3]; //Position of the gameobject
        float rotation[4]; //Rotation of the gameobject (Quaternion)
        float scale[3]; //Scale of the gameobject
        Syngine::Components getComponentType() override;
        void GetRotationAsEuler(float& x, float& y, float& z) const; //Get the rotation of the gameobject as Euler angles (in radians)

        void SetPosition(float x, float y, float z); //Set the position of the gameobject
        void SetRotation(float x, float y, float z); //Set the rotation of the gameobject (Euler XYZ)
        void SetRotation(float x, float y, float z, float w); //Set the rotation of the gameobject (Quaternion)
        void SetScale(float x, float y, float z); //Set the scale of the gameobject

        void
        GetModelMatrix(float* result); // Get the model matrix of the gameobject
        const float* GetPosition(); // Get the position of the gameobject
};

} // namespace Syngine