$input v_texcoord0

#include <bgfx_shader.sh>

SAMPLER2D(s_ssao, 0);
SAMPLER2D(s_depth, 1);
SAMPLER2D(s_normal, 2);
uniform vec4 u_floats; // x = radius, y = vertical (1.0) or horizontal (0.0) blur, z = width, w = unused

void main() {
    vec2 uv = v_texcoord0;
    float ao = 0.0;
    float weightSum = 0.0;

    vec2 texelSize = u_viewTexel.xy; 
    vec2 direction = vec2(0.0, 1.0);
    if (u_floats.y < 0.5) {
        direction = vec2(1.0, 0.0);
    } else {
        direction = vec2(0.0, 1.0);
    }
    vec2 stride = direction * texelSize * u_floats.x; // Multiply by radius

    float centerDepth = texture2D(s_depth, uv).r;
    vec3 centerNormal = texture2D(s_normal, uv).xyz * 2.0 - 1.0;
    float centerAO = texture2D(s_ssao, uv).r;
    ao += centerAO;
    weightSum += 1.0;

    float sigma = u_floats.x / 2.0;
    float twoSigmaSq = 2.0 * sigma * sigma;

    // Depth and normal based bilateral blur
    for (int i = -4; i <= 4; i++) {
        if (i == 0) continue; // Skip center sample

        vec2 offset = direction * float(i) * texelSize * u_floats.x;

        float sampleAO = texture2D(s_ssao, uv + offset).r;
        float sampleDepth = texture2D(s_depth, uv + offset).r;
        vec3 sampleNormal = texture2D(s_normal, uv + offset).xyz * 2.0 - 1.0;

        float w = exp(-float(i*i) / twoSigmaSq); // Gaussian weight

        float depthDiff = abs(sampleDepth - centerDepth);
        w *= smoothstep(0.2, 0.0, depthDiff); // Depth weight (softened)

        float normalDiff = max(dot(sampleNormal, centerNormal), 0.0);
        w *= pow(normalDiff, 4.0); // Normal weight (stricter to preserve edges)

        ao += sampleAO * w;
        weightSum += w;
    }
    float finalAO = ao / max(weightSum, 0.0001);
    gl_FragColor = vec4(finalAO, finalAO, finalAO, 1.0);
}
