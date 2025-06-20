$input a_position
#include <bgfx_shader.sh>

// u_model is the world matrix (model matrix)
// u_view is the view matrix
// u_projection is the projection matrix
// u_modelViewProj is u_proj * u_view * u_model

void main() {
    gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));
}