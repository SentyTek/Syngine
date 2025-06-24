$input a_position, a_normal
$output v_worldNormal
#include <bgfx_shader.sh>

// u_model is the world matrix (model matrix)
// u_view is the view matrix
// u_projection is the projection matrix
// u_modelViewProj is u_proj * u_view * u_model

uniform mat3 u_normalMatrix; // should be the unverse transpose of the model matrix

void main() {
    gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));
    // transform normal to world space
    v_worldNormal = normalize(mul(u_normalMatrix, a_normal));
}