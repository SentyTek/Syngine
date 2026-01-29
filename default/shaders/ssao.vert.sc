$output v_texcoord0
#include <bgfx_shader.sh>

void main()
{
    vec2 pos = vec2(
        gl_VertexID == 1 ? 3.0 : -1.0,
        gl_VertexID == 2 ? 3.0 : -1.0
    );
    
    v_texcoord0 = pos * 0.5 + 0.5;
    gl_Position = vec4(pos, 0.0, 1.0);
}
