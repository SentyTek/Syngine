$input a_position a_normal
$output v_worldPos v_worldNormal v_viewDepth
#include <bgfx_shader.sh>

// u_model is the world matrix (model matrix)
// u_view is the view matrix
// u_projection is the projection matrix
// u_modelViewProj is u_proj * u_view * u_model

uniform mat3 u_normalMatrix; // should be the inverse transpose of the model matrix

void main() {
    v_worldPos = mul(u_model[0], vec4(a_position, 1.0)).xyz;
    // view-space depth for cascade selection (z in view space)
    v_viewDepth = mul(u_view, vec4(v_worldPos, 1.0)).z;
    gl_Position = mul(u_viewProj, vec4(v_worldPos, 1.0));
    // transform normal to world space
    v_worldNormal = normalize(mul(u_normalMatrix, a_normal));
}