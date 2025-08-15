$input a_position a_normal a_tangent a_texcoord0 a_texcoord1
$output v_uvMacro v_uvDetail v_normal v_tangent
#include <bgfx_shader.sh>

uniform mat3 u_normalMatrix;

void main() {
    //position
    vec4 worldPos   = mul(u_modelViewProj, vec4(a_position, 1.0));
    gl_Position     = worldPos;

    //two UV sets
    v_uvMacro   = a_texcoord0; //0->1 for heightmap diffs
    v_uvDetail  = a_texcoord1; //already pre-scaled or 1->1 for detail maps

    //normals/tangents
    v_normal    = normalize( mul(u_normalMatrix, a_normal) );
    vec3 t      = normalize( mul(u_normalMatrix, a_tangent.xyz) );
    v_tangent   = vec4( t, a_tangent.w );
}
