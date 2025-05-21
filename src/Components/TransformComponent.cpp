#include "TransformComponent.h"

TransformComponent::TransformComponent() {
    position[0] = position[1] = position[2] = 0.0f;
    rotation[0] = rotation[1] = rotation[2] = 0.0f;
    scale[0] = scale[1] = scale[2] = 1.0f;
}

SynComponents TransformComponent::getComponentType() {
    return SYN_COMPONENT_TRANSFORM;
}

void TransformComponent::SetPosition(float x, float y, float z) {
    position[0] = x;
    position[1] = y;
    position[2] = z;
}

void TransformComponent::SetRotation(float x, float y, float z) {
    rotation[0] = x;
    rotation[1] = y;
    rotation[2] = z;
}

void TransformComponent::SetScale(float x, float y, float z) {
    scale[0] = x;
    scale[1] = y;
    scale[2] = z;
}