$input v_texcoord0
#include <bgfx_shader.sh>

SAMPLER2D(s_albedo, 0);

void main() {
    // Sample the albedo texture
    vec4 base = texture2D(s_albedo, v_texcoord0);

    // Alpha Test: Discard the fragment if its alpha is below a threshold.
    // This allows for proper transparency handling in billboards.
    if (base.a < 0.1) {
        discard;
    }

    // Output the color
    gl_FragColor = base;
}
