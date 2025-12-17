$input a_position a_normal a_color0
$output v_worldPos v_worldNormal v_viewDepth v_vertexColor
#include <bgfx_shader.sh>

// u_model is the world matrix (model matrix)
// u_view is the view matrix
// u_projection is the projection matrix
// u_modelViewProj is u_proj * u_view * u_model

uniform mat3 u_normalMatrix; // should be the inverse transpose of the model matrix

void main() {
    // Transform position to world space
    vec4 worldPos = mul(u_model[0], vec4(a_position, 1.0));
    v_worldPos = worldPos.xyz;
    v_viewDepth = mul(u_view, worldPos).z;
    v_worldNormal = normalize(mul(u_normalMatrix, a_normal));
    v_vertexColor = a_color0;
    
    gl_Position = mul(u_viewProj, worldPos);
}
