$input a_position, a_texcoord0
$output v_texcoord0
#include <bgfx_shader.sh>

uniform vec4 u_default_billboard;      // xyz = center, w = size
uniform vec4 u_default_billboard_mode; // xyz = rot (fixed), w = mode
uniform vec4 u_cameraPos;

// Helper functions for manual vector rotation
// This avoids potential matrix construction issues in cross-compilation
vec3 rotateX(vec3 v, float angle) {
    float s = sin(angle);
    float c = cos(angle);
    return vec3(
        v.x,
        v.y * c - v.z * s,
        v.y * s + v.z * c
    );
}

vec3 rotateY(vec3 v, float angle) {
    float s = sin(angle);
    float c = cos(angle);
    return vec3(
        v.x * c + v.z * s,
        v.y,
        -v.x * s + v.z * c
    );
}

vec3 rotateZ(vec3 v, float angle) {
    float s = sin(angle);
    float c = cos(angle);
    return vec3(
        v.x * c - v.y * s,
        v.x * s + v.y * c,
        v.z
    );
}

void main() {
    vec3 center = u_default_billboard.xyz;
    float size  = u_default_billboard.w;
    float mode  = u_default_billboard_mode.w;

    vec3 right;
    vec3 up;
    vec3 offset;

    if (mode < 0.5) {
        // FULL BILLBOARD - extract camera right/up from inverse view matrix
        right = normalize(vec3(u_view[0][0], u_view[0][1], u_view[0][2]));
        up    = normalize(vec3(u_view[1][0], u_view[1][1], u_view[1][2]));
        
        offset = (right * a_position.x + up * a_position.y) * size;

    } else if (mode < 1.5) {
        // Y-LOCKED (vertical axis)
        up = vec3(0.0, 1.0, 0.0);
        right = normalize(vec3(u_view[0][0], u_view[0][1], u_view[0][2]));
        
        // Recalculate forward to ensure orthogonality
        vec3 forward = normalize(cross(right, up));
        // Actually use the corrected vectors
        up = normalize(cross(forward, right));
        
        offset = (right * a_position.x + up * a_position.y) * size;

    } else {
        // FIXED WITH ROTATION
        // Manually rotate the local position vector based on provided Euler angles. Rotate in ZYX order.
        vec3 localPos = vec3(a_position.x, a_position.y, 0.0);
        
        vec3 r = rotateZ(localPos, u_default_billboard_mode.z);
        r = rotateY(r, u_default_billboard_mode.y);
        r = rotateX(r, u_default_billboard_mode.x);
        
        offset = r * size;
    }
    
    // Apply billboard transformation
    vec3 worldPos = center + offset;

    gl_Position = mul(u_viewProj, vec4(worldPos, 1.0));
    v_texcoord0 = a_texcoord0;
}
