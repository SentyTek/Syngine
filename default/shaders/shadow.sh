SAMPLER2D(s_shadowMap, 3);
uniform vec4 u_csmSplits;
uniform mat4 u_csmLightViewProj[4];
uniform vec4 u_viewPos;
uniform vec4 u_shadowParams; // x = homogeneous depth?, y = shadow map size, z = light far plane, w unused.

// Simple hash function for randomization
float rand(vec2 co) {
    return fract(sin(dot(co.xy, vec2(12.9898,78.233))) * 43758.5453);
}

float sampleShadowPCF3x3(vec2 uv, float compareDepth, float bias, float texelSize) {
    float shadow = 0.0;

    // 3x3 PCF sampling w/ random offset
    // I wish I could use for loops here (thanks bgfx)
    vec2 baseUV;
    float r;

    baseUV = uv + vec2(-1.0, -1.0) * texelSize;
    r = (rand(baseUV) - 0.5) * texelSize;
    shadow += step(0.0, compareDepth - bias - texture2D(s_shadowMap, baseUV + r).r);

    baseUV = uv + vec2( 0.0, -1.0) * texelSize;
    r = (rand(baseUV) - 0.5) * texelSize;
    shadow += step(0.0, compareDepth - bias - texture2D(s_shadowMap, baseUV + r).r);

    baseUV = uv + vec2( 1.0, -1.0) * texelSize;
    r = (rand(baseUV) - 0.5) * texelSize;
    shadow += step(0.0, compareDepth - bias - texture2D(s_shadowMap, baseUV + r).r);

    baseUV = uv + vec2(-1.0,  0.0) * texelSize;
    r = (rand(baseUV) - 0.5) * texelSize;
    shadow += step(0.0, compareDepth - bias - texture2D(s_shadowMap, baseUV + r).r);

    baseUV = uv + vec2( 0.0,  0.0) * texelSize;
    r = (rand(baseUV) - 0.5) * texelSize;
    shadow += step(0.0, compareDepth - bias - texture2D(s_shadowMap, baseUV + r).r);

    baseUV = uv + vec2( 1.0,  0.0) * texelSize;
    r = (rand(baseUV) - 0.5) * texelSize;
    shadow += step(0.0, compareDepth - bias - texture2D(s_shadowMap, baseUV + r).r);

    baseUV = uv + vec2(-1.0,  1.0) * texelSize;
    r = (rand(baseUV) - 0.5) * texelSize;
    shadow += step(0.0, compareDepth - bias - texture2D(s_shadowMap, baseUV + r).r);

    baseUV = uv + vec2( 0.0,  1.0) * texelSize;
    r = (rand(baseUV) - 0.5) * texelSize;
    shadow += step(0.0, compareDepth - bias - texture2D(s_shadowMap, baseUV + r).r);

    baseUV = uv + vec2( 1.0,  1.0) * texelSize;
    r = (rand(baseUV) - 0.5) * texelSize;
    shadow += step(0.0, compareDepth - bias - texture2D(s_shadowMap, baseUV + r).r);

    return shadow / 9.0; // Average of 9 samples
}

float getShadowFactor(vec3 worldPos, vec3 worldNormal, vec4 lightDir, float viewDepth) {
    // Simple cascade selection based on distance from camera
    int c = 0;
    if (viewDepth > u_csmSplits.x) c = 1;
    if (viewDepth > u_csmSplits.y) c = 2;
    if (viewDepth > u_csmSplits.z) c = 3;
    
    // Normal offset to reduce shadow acne
    vec3 lightDirNorm = normalize(lightDir.xyz);
    vec3 normalNorm = normalize(worldNormal);
    float NdotL = dot(normalNorm, lightDirNorm);
    
    // Clamp NdotL to prevent extreme values on back-facing surfaces
    NdotL = clamp(NdotL, 0.01, 1.0);
    
    // Calculate normal offset based on cascade and surface angle
    float normalOffsetScale = (1.0 + float(c)) * 0.1; // Increase offset for farther cascades
    float normalOffset = normalOffsetScale * (1.0 - NdotL); // More offset for steeper angles
    
    // Apply normal offset to world position before projection
    vec3 offsetWorldPos = worldPos + normalNorm * normalOffset;
    
    vec4 sc = mul(u_csmLightViewProj[c], vec4(offsetWorldPos, 1.0));
    
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

    // Enhanced depth bias calculation
    float baseBias = 0.0001;
    float maxBias = 0.01;
    
    // Cascade-dependent bias (closer cascades need less bias)
    float cascadeBias = baseBias * (1.0 + float(c) * 0.5);
    
    // Surface angle bias - use a more sophisticated calculation
    float slopeBias = cascadeBias * tan(acos(NdotL));
    slopeBias = clamp(slopeBias, 0.0, maxBias * 0.5);
    
    // Add receiver plane depth bias for more stable shadows
    vec2 texelWorldSize = vec2_splat(u_shadowParams.y) / pow(2.0, float(c)); // Texel size in world units for this cascade
    float receiverPlaneDepthBias = abs(dFdx(normalizedZ)) + abs(dFdy(normalizedZ));
    receiverPlaneDepthBias *= texelWorldSize.x * 0.5;
    
    float totalBias = cascadeBias + slopeBias + receiverPlaneDepthBias;
    totalBias = min(totalBias, maxBias); // Cap maximum bias

    float texelSize = u_shadowParams.y * 0.5; // Half texel for PCF
    
    // Use 3x3 PCF for higher quality (more expensive)
    float shadow = sampleShadowPCF3x3(uv, normalizedZ, totalBias, texelSize);

    // Additional smoothing for very low angles
    if (NdotL < 0.08) {
        // Fade out shadows on very steep angles to prevent artifacts
        float fadeOut = smoothstep(0.01, 0.08, NdotL);
        shadow *= fadeOut;
    }

    // Based on light elevation, everything should be in shadow when sun is low or negative
    float sunElevation = lightDir.y;
    if (sunElevation < 0.0) {
        return 1.0;
    }
    return 1.0 - shadow * 0.7;
}
