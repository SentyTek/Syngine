#pragma once
#include "Components.h"

/*
    Syngine Transform Component
    The TransformComponent is used to represent the position, rotation, and scale of a gameobject in the game world.
*/
class TransformComponent : public SynComponent {
    public:
        static constexpr SynComponents componentType = SYN_COMPONENT_TRANSFORM;

        TransformComponent();
        float position[3]; //Position of the gameobject
        float rotation[3]; //Rotation of the gameobject
        float scale[3]; //Scale of the gameobject
        SynComponents getComponentType() override;

        void SetPosition(float x, float y, float z); //Set the position of the gameobject
        void SetRotation(float x, float y, float z); //Set the rotation of the gameobject
        void SetScale(float x, float y, float z); //Set the scale of the gameobject

        float* GetModelMatrix(); //Get the model matrix of the gameobject
};