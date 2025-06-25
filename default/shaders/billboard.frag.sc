$input v_texcoord0
#include <bgfx_shader.sh>

SAMPLER2D(s_albedo, 0);

void main() {
    // Sample the albedo texture
    vec4 base = texture2D(s_albedo, v_texcoord0);

    if (base.a < 0.1) {
        // If the alpha is very low, discard the fragment
        discard;
    }

    // Output the color
    gl_FragColor = base;
}