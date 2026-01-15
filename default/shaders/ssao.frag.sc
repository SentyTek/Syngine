// https://www.activision.com/cdn/research/Practical_Real_Time_Strategies_for_Accurate_Indirect_Occlusion_NEW%20VERSION_COLOR.pdf

$input v_texcoord0

#include <bgfx_shader.sh>

SAMPLER2D(s_depth, 0);
SAMPLER2D(s_normal, 1);
SAMPLER2D(s_noise, 2);

uniform vec4 u_ssaoParams; // x: radius, y: bias, z: intensity, w: width

// Reconstruct view space position from depth
vec3 reconstructPosition(vec2 uv, float depth) {
    float z = depth * 2.0 - 1.0;
    vec4 clipSpacePos = vec4(uv * 2.0 - 1.0, z, 1.0);
    vec4 viewSpacePos = mul(u_invProj, clipSpacePos);
    return viewSpacePos.xyz / viewSpacePos.w;
}

void main() {
    vec2 uv = v_texcoord0;
    uv.y = 1.0 - uv.y; // Flip Y for texture lookup

    // Get depth and normal
    vec4 worldNormal = texture2D(s_normal, uv) * 2.0 - 1.0;
    vec3 normal = normalize(mul(u_view, worldNormal).xyz);
    float depth = texture2D(s_depth, uv).x;

    // Early out for far plane
    if (depth >= 1.0) {
        gl_FragColor = vec4_splat(1.0);
        return;
    }

    // Reconstruct position
    vec3 pos = reconstructPosition(uv, depth);

    // SSAO calculation
    // TODO: Convert to XeGTAO once compute is online
    // Sample in a hemisphere around the normal
    vec2 noiseScale = vec2(u_ssaoParams.w / 4.0, u_ssaoParams.w / 4.0);
    vec3 randomVec = texture2D(s_noise, uv * noiseScale).xyz * 2.0 - 1.0;
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    float occlusion = 0.0;
    int sampleCount = 16;
    UNROLL for (int i = 0; i < sampleCount; ++i) {
        float angle = float(i) / float(sampleCount) * 6.28318530718; // tau
        float scale = float(i) / float(sampleCount);
        scale = mix(0.1, 1.0, scale * scale);
        float radius = u_ssaoParams.x * scale;
        vec3 sampleVec = vec3(cos(angle), sin(angle), sqrt(1.0 - cos(angle)*cos(angle))) * radius;
        sampleVec = mul(TBN, sampleVec); // Transform to view space

        vec3 samplePos = pos + sampleVec;
        vec4 offset = mul(u_proj, vec4(samplePos, 1.0));
        offset.xyz /= offset.w;
        vec2 sampleUV = offset.xy * 0.5 + 0.5;
        //sampleUV.y = 1.0 - sampleUV.y; // Not needed due to earlier flip

        float sampleDepth = texture2D(s_depth, sampleUV).x;
        
        vec3 geometryPos = reconstructPosition(sampleUV, sampleDepth);

        float rangeCheck = smoothstep(0.0, 1.0, u_ssaoParams.x / abs(pos.z - geometryPos.z));

        float delta = geometryPos.z - samplePos.z;
        occlusion += step(u_ssaoParams.y, delta) * rangeCheck;
    }

    gl_FragColor = vec4(vec3_splat(1.0 - (occlusion / float(sampleCount))), 1.0);
}
