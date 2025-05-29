#pragma once
#include "Components.h"
#include "SyngineGameobject.h"

/*
    Syngine Transform Component
    The TransformComponent is used to represent the position, rotation, and scale of a gameobject in the game world.
*/
class TransformComponent : public SynComponent {
    public:
        static constexpr SynComponents componentType = SYN_COMPONENT_TRANSFORM;

        TransformComponent(GameObject* owner);
        float position[3]; //Position of the gameobject
        float rotation[4]; //Rotation of the gameobject (Quaternion)
        float scale[3]; //Scale of the gameobject
        SynComponents getComponentType() override;
        void GetRotationAsEuler(float& x, float& y, float& z) const; //Get the rotation of the gameobject as Euler angles (in radians)

        void SetPosition(float x, float y, float z); //Set the position of the gameobject
        void SetRotation(float x, float y, float z); //Set the rotation of the gameobject (Euler XYZ)
        void SetRotation(float x, float y, float z, float w); //Set the rotation of the gameobject (Quaternion)
        void SetScale(float x, float y, float z); //Set the scale of the gameobject

        void GetModelMatrix(float* result); //Get the model matrix of the gameobject
};