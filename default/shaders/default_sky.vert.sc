$output v_worldViewRay
#include <bgfx_shader.sh>

void main() {
    vec2 pos = vec2(
        gl_VertexID == 1 ? 3.0 : -1.0,
        gl_VertexID == 2 ? 3.0 : -1.0
    );
    
    gl_Position = vec4(pos, 1.0, 1.0);
    
    vec4 viewPos = mul(u_invProj, gl_Position);
    v_worldViewRay = mul(u_invView, vec4(viewPos.xyz / viewPos.w, 0.0)).xyz;
}
