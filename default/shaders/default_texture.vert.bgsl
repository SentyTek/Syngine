$input a_position a_normal a_tangent a_texcoord0 a_texcoord1
$output v_uvMacro v_uvDetail v_normal v_tangent v_worldPos v_viewDepth
#include <bgfx_shader.sh>

uniform mat3 u_normalMatrix;

void main() {
    //position
    v_worldPos   = mul(u_model[0], vec4(a_position, 1.0)).xyz;
    v_viewDepth   = mul(u_view, vec4(v_worldPos, 1.0)).z;
    gl_Position     = mul(u_viewProj, vec4(v_worldPos, 1.0));

    //two UV sets
    v_uvMacro   = a_texcoord0; //0->1 for heightmap diffs
    v_uvDetail  = a_texcoord1; //already pre-scaled or 1->1 for detail maps

    //normals/tangents
    v_normal    = normalize( mul(u_normalMatrix, a_normal) );
    vec3 t      = normalize( mul(u_normalMatrix, a_tangent.xyz) );
    v_tangent   = vec4( t, a_tangent.w );
}
