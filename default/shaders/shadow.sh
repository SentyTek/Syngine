SAMPLER2D(s_shadowMap, 3);
uniform vec4 u_csmSplits;
uniform mat4 u_csmLightViewProj[4];
uniform vec4 u_viewPos;
uniform vec4 u_shadowParams;

float sampleShadowPCF3x3(vec2 uv, float compareDepth, float bias, float texelSize) {
    float shadow = 0.0;
    
    // 3x3 PCF sampling - manually unrolled
    shadow += step(0.0, compareDepth - bias - texture2D(s_shadowMap, uv + vec2(-1.0, -1.0) * texelSize).r);
    shadow += step(0.0, compareDepth - bias - texture2D(s_shadowMap, uv + vec2( 0.0, -1.0) * texelSize).r);
    shadow += step(0.0, compareDepth - bias - texture2D(s_shadowMap, uv + vec2( 1.0, -1.0) * texelSize).r);
    
    shadow += step(0.0, compareDepth - bias - texture2D(s_shadowMap, uv + vec2(-1.0,  0.0) * texelSize).r);
    shadow += step(0.0, compareDepth - bias - texture2D(s_shadowMap, uv + vec2( 0.0,  0.0) * texelSize).r);
    shadow += step(0.0, compareDepth - bias - texture2D(s_shadowMap, uv + vec2( 1.0,  0.0) * texelSize).r);
    
    shadow += step(0.0, compareDepth - bias - texture2D(s_shadowMap, uv + vec2(-1.0,  1.0) * texelSize).r);
    shadow += step(0.0, compareDepth - bias - texture2D(s_shadowMap, uv + vec2( 0.0,  1.0) * texelSize).r);
    shadow += step(0.0, compareDepth - bias - texture2D(s_shadowMap, uv + vec2( 1.0,  1.0) * texelSize).r);
    
    return shadow / 9.0; // Average of 9 samples
}

float getShadowFactor(vec3 worldPos, vec3 worldNormal, vec4 lightDir, float viewDepth) {
    // Simple cascade selection based on distance from camera
    int c = 0;
    if (viewDepth > u_csmSplits.x) c = 1;
    if (viewDepth > u_csmSplits.y) c = 2;
    if (viewDepth > u_csmSplits.z) c = 3;
    //return float(c) / 3.0;
    vec4 sc = mul(u_csmLightViewProj[c], vec4(worldPos, 1.0));
    
    if (abs(sc.w) < 0.001) {
        return 1.0; // Fully lit if projection fails
    }

    // Perspective divide
    vec3 proj = sc.xyz / sc.w;
    vec2 projXY = proj.xy * 0.5 + 0.5;

    // Flip Y coordinate for D3D, to match DirectX's coordinate system
#if BGFX_SHADER_LANGUAGE_HLSL
    projXY.y = 1.0 - projXY.y;
#endif

    float normalizedZ = proj.z;

    // Check if fragment is outside light frustum
    if (any(lessThan(projXY, vec2_splat(0.0))) || any(greaterThan(projXY, vec2_splat(1.0))) || 
        normalizedZ < 0.0 || normalizedZ > 1.0) {
        return 1.0; // Outside frustum, fully lit
    }

    // Calculate atlas UV coordinates for 2x2 cascade layout
    vec2 uvOffset;
    if (c == 0) uvOffset = vec2(0.0, 0.0);        // Top-left
    else if (c == 1) uvOffset = vec2(0.5, 0.0);   // Top-right
    else if (c == 2) uvOffset = vec2(0.0, 0.5);   // Bottom-left
    else uvOffset = vec2(0.5, 0.5);               // Bottom-right
    
    vec2 uv = projXY * 0.5 + uvOffset;

    // Bias calculation for parallel surfaces
    vec3 lightDirNorm = normalize(lightDir.xyz);
    vec3 normalNorm = normalize(worldNormal);
    float NdotL = dot(normalNorm, lightDirNorm);
    
    // Clamp NdotL to prevent extreme values on back-facing surfaces
    NdotL = clamp(NdotL, 0.01, 1.0);
    
    // Calculate bias based on surface angle to light
    float baseBias = 0.0008;
    float maxBias = 0.01;
    
    // Use tangent-based bias calculation for better handling of grazing angles
    float tanTheta = sqrt(1.0 - NdotL * NdotL) / NdotL;
    float bias = clamp(baseBias + baseBias * tanTheta, baseBias, maxBias);
    
    // Additional cascade-dependent bias scaling
    float cascadeScale = 1.0 + float(c) * 0.5; // 1.0, 1.5, 2.0, 2.5
    bias *= cascadeScale;

    float texelSize = u_shadowParams.y * 0.5; // Half texel for PCF
    
    // Use 3x3 PCF for higher quality (more expensive)
    float shadow = sampleShadowPCF3x3(uv, normalizedZ, bias, texelSize);
    
    // Or use 2x2 PCF for better performance
    // float shadow = sampleShadowPCF2x2(uv, normalizedZ, bias, texelSize);
    
    // Additional smoothing for very low angles
    if (NdotL < 0.1) {
        // Fade out shadows on very steep angles to prevent artifacts
        float fadeOut = smoothstep(0.01, 0.1, NdotL);
        shadow *= fadeOut;
    }
    
    // Blend between lit and shadowed
    return 1.0 - shadow * 0.7;
}
