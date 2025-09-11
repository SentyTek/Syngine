$input a_position, a_texcoord0
$output v_texcoord0
#include <bgfx_shader.sh>

// x,y,z = world center, w = size
uniform vec4 u_default_billboard;

void main() {
    // Billbord world center to view space
    vec4 viewCenter = mul(u_view, vec4(u_default_billboard.xyz, 1.0));

    // Add local quad offsets in view space.
    // Quad should always be parallel to the camera plane.
    vec4 viewPos = viewCenter + vec4(mul(a_position.x, u_default_billboard.w), mul(a_position.y, u_default_billboard.w), 0.0, 0.0);

    // Transform into clip space
    gl_Position = mul(u_proj, viewPos);

    v_texcoord0 = a_texcoord0; // Pass through the texture coordinates
}