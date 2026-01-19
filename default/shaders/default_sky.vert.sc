$output v_worldViewRay
#include <bgfx_shader.sh>

void main() {
    vec2 pos = vec2(
        gl_VertexID == 1 ? 3.0 : -1.0,
        gl_VertexID == 2 ? 3.0 : -1.0
    );

    gl_Position = vec4(pos, 1.0, 1.0);

    vec4 worldPos = mul(u_invViewProj, gl_Position);

    worldPos /= worldPos.w;

    // bgfx standard uniform for camera position is u_view[3] or u_invView[3] depending on setup,
    // but calculating direction via world coordinates is safest.
    vec3 camPos = mul(u_invView, vec4(0.0, 0.0, 0.0, 1.0)).xyz;
    v_worldViewRay = worldPos.xyz - camPos;
}
