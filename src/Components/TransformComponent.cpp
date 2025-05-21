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

float* TransformComponent::GetModelMatrix() {
    static float model[16];
    // Calculate the model matrix based on position, rotation, and scale
    // Probably implement a proper matrix library for this
    model[0] = scale[0]; model[1] = 0.0f; model[2] = 0.0f; model[3] = 0.0f;
    model[4] = 0.0f; model[5] = scale[1]; model[6] = 0.0f; model[7] = 0.0f;
    model[8] = 0.0f; model[9] = 0.0f; model[10] = scale[2]; model[11] = 0.0f;
    model[12] = position[0]; model[13] = position[1]; model[14] = position[2]; model[15] = 1.0f;
    return model;
}