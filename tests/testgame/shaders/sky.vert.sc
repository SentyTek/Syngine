$output v_worldViewRay
#include <bgfx_shader.sh>

void main() {
    float x, y;
    if(gl_VertexID == 0) {
        x = -1.0;
        y = -1.0;
    } else if(gl_VertexID == 1) {
        x = 3.0;
        y = -1.0;
    } else {
        x = -1.0;
        y = 3.0;
    }
    gl_Position = vec4(x, y, 1.0, 1.0);

    vec4 ndcPos = gl_Position;
    vec4 viewPos = mul(u_invProj, ndcPos);
    viewPos /= viewPos.w;

    vec3 worldDir = mul(u_invView, vec4(viewPos.xyz, 0.0)).xyz;
    v_worldViewRay = worldDir;
}