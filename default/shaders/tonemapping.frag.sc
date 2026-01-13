$input v_texcoord0
#include <bgfx_shader.sh>

SAMPLER2D(s_sceneColor, 0); // RGBA16F
SAMPLER2D(s_ssao, 1);       // R8

void main() {
    vec2 uv = v_texcoord0;
    
    vec3 color = texture2D(s_sceneColor, uv).rgb;
    float ssao = texture2D(s_ssao, uv).r;
    
    // Apply SSAO (Multiplicative approximation)
    color *= pow(ssao, 2.0); // 2.0 is an arbitrary strength factor

    // ACES Tone Mapping
    float a = 2.51;
    float b = 0.03;
    float c = 2.43;
    float d = 0.59;
    float e = 0.14;
    vec3 toneMapped = clamp((color*(a*color+b))/(color*(c*color+d)+e), 0.0, 1.0);

    // Gamma Correction
    vec3 final = pow(toneMapped, vec3_splat(1.0/2.2));

    gl_FragColor = vec4(final, 1.0);
}