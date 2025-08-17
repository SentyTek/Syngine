SAMPLER2D(s_shadowMap, 3);
uniform vec4 u_csmSplits;
uniform mat4 u_csmLightViewProj[4];
uniform vec4 u_viewPos;

float getShadowFactor(vec3 worldPos, vec3 v_worldNormal, vec4 u_lightDir, float viewDepth) {
    // viewDepth should be camera-space Z (positive forward)
    float depth = viewDepth;

    uint cascadeIndex = 0u;
    if (depth > u_csmSplits.x) cascadeIndex = 1u;
    if (depth > u_csmSplits.y) cascadeIndex = 2u;
    if (depth > u_csmSplits.z) cascadeIndex = 3u;

    vec4 shadowCoord = mul(u_csmLightViewProj[cascadeIndex], vec4(worldPos, 1.0));
    vec3 projCoord = shadowCoord.xyz / shadowCoord.w;
    projCoord = projCoord * 0.5 + 0.5; // NDC [-1,1] -> [0,1]

    // atlas mapping (2x2)
    vec2 cascade_uv_scale  = vec2(0.5, 0.5);
    vec2 cascade_uv_offset = vec2(float(cascadeIndex % 2u) * 0.5, float(cascadeIndex / 2u) * 0.5);
    vec2 uv = projCoord.xy * cascade_uv_scale + cascade_uv_offset;

    // Clamp to cascade bounds to prevent bleeding
    vec2 cascadeMin = cascade_uv_offset;
    vec2 cascadeMax = cascade_uv_offset + cascade_uv_scale;
    uv = clamp(uv, cascadeMin, cascadeMax);

    // outside cascade -> no shadow
    if (projCoord.x < 0.0 || projCoord.x > 1.0 || projCoord.y < 0.0 || projCoord.y > 1.0 || 
        projCoord.z < 0.0 || projCoord.z > 1.0) {
        return 1.0;
    }

    // bias: small, based on angle between normal and light direction
    float NdotL = max(dot(normalize(v_worldNormal), normalize(u_lightDir.xyz)), 0.0);
    float bias = max(0.001 * (1.0 - NdotL), 0.0001);

    float shadowDepth = texture2D(s_shadowMap, uv).r;

    // If fragment is further from light than stored depth + bias -> in shadow
    bool inShadow = projCoord.z > (shadowDepth + bias);

    return inShadow ? 0.4 : 1.0;
}