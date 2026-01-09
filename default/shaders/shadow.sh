SAMPLER2D(s_shadowMap, 3);
uniform vec4 u_csmSplits;
uniform mat4 u_csmLightViewProj[4];
uniform vec4 u_viewPos;
uniform vec4 u_shadowParams; // x = homogeneous depth?, y = shadow map size, z = light far plane, w unused.
uniform vec4 u_csmTexelSize; // x = casc0, y = casc1, z = casc2, w = casc3

float sampleShadow2x2PCF(vec2 uv, float compareDepth, float bias, float texelSize) {
    float shadow = 0.0;

    UNROLL for (int y = -1; y <= 0; y++) {
        UNROLL for (int x = -1; x <= 0; x++) {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            float sampleDepth = texture2D(s_shadowMap, uv + offset).r;
            shadow += step(0.0, compareDepth - bias - sampleDepth);
        }
    }
    return shadow / 4.0;
}

float sampleShadowPoisson8(vec2 uv, float compareDepth, float bias, float texelSize) {
    // Must be defined in a function scope in BGFX
    vec2 poissonDisk8[8];
    poissonDisk8[0] = vec2(-0.7071, 0.7071);
    poissonDisk8[1] = vec2(-0.0000, -0.8750);
    poissonDisk8[2] = vec2(0.5303, 0.5303);
    poissonDisk8[3] = vec2(-0.6250, -0.0000);
    poissonDisk8[4] = vec2(0.3536, -0.3536);
    poissonDisk8[5] = vec2(-0.3536, 0.3536);
    poissonDisk8[6] = vec2(0.8750, 0.0000);
    poissonDisk8[7] = vec2(0.0000, 0.6250);

    float shadow = 0.0;

    UNROLL for (int i = 0; i < 8; i++) {
        vec2 sampleUV = uv + poissonDisk8[i] * texelSize;
        float sampleDepth = texture2D(s_shadowMap, sampleUV).r;
        shadow += step(0.0, compareDepth - bias - sampleDepth);
    }
    return shadow / 8.0;
}

float sampleShadowPoisson16(vec2 uv, float compareDepth, float bias, float texelSize) {
    // Must be defined in a function scope in BGFX
    vec2 poissonDisk16[16];
    poissonDisk16[0] = vec2(-0.94201624, -0.39906216);
    poissonDisk16[1] = vec2(0.94558609, -0.76890725);
    poissonDisk16[2] = vec2(-0.09418410, -0.92938870);
    poissonDisk16[3] = vec2(0.34495938, 0.29387760);
    poissonDisk16[4] = vec2(-0.91588581, 0.45771432);
    poissonDisk16[5] = vec2(-0.81544232, -0.87912464);
    poissonDisk16[6] = vec2(-0.38277543, 0.27676845);
    poissonDisk16[7] = vec2(0.97484398, 0.75648379);
    poissonDisk16[8] = vec2(0.44323325, -0.97511554);
    poissonDisk16[9] = vec2(0.53742981, -0.47373420);
    poissonDisk16[10] = vec2(-0.26496911, -0.41893023);
    poissonDisk16[11] = vec2(0.79197514, 0.19090188);
    poissonDisk16[12] = vec2(-0.24188840, 0.99706507);
    poissonDisk16[13] = vec2(-0.81409955, 0.91437590);
    poissonDisk16[14] = vec2(0.19984126, 0.78641367);
    poissonDisk16[15] = vec2(0.14383161, -0.14100790);
    float shadow = 0.0;

    UNROLL for (int i = 0; i < 16; i++) {
        vec2 sampleUV = uv + poissonDisk16[i] * texelSize;
        float sampleDepth = texture2D(s_shadowMap, sampleUV).r;
        shadow += step(0.0, compareDepth - bias - sampleDepth);
    }
    return shadow / 16.0;
}

float getShadowFactor(vec3 worldPos, vec3 geomNormal, vec3 shadeNormal, vec4 lightDir, float viewDepth) {
    // Simple cascade selection based on distance from camera
    int c = 0;
    float fade = 0.0;
    float blend = 5.0;
    float currentTexelSize = u_csmTexelSize.x;

    // Better cascade selection with smoother transition zones
    if (viewDepth > u_csmSplits.x) { 
        c = 1; 
        fade = smoothstep(u_csmSplits.x, u_csmSplits.x + blend, viewDepth); 
    }
    
    if (viewDepth > u_csmSplits.y) { 
        c = 2; 
        fade = smoothstep(u_csmSplits.y, u_csmSplits.y + blend, viewDepth); 
    }
    
    if (viewDepth > u_csmSplits.z) { 
        c = 3; 
        fade = smoothstep(u_csmSplits.z, u_csmSplits.z + blend, viewDepth); 
    }
    
    // Normal offset to reduce shadow acne
    vec3 lightDirNorm = normalize(lightDir.xyz);
    vec3 geomN = normalize(geomNormal);

    // Clamp NdotL to avoid extreme angles
    float NdotL = dot(geomN, lightDirNorm);
    NdotL = clamp(NdotL, 0.01, 1.0);
    
    // Offset along normal based on angle to light
    float normalOffset = currentTexelSize * 3.0 * (1.0 - NdotL);
    vec3 offsetWorldPos = worldPos + geomN * normalOffset;
    
    vec4 sc = mul(u_csmLightViewProj[c], vec4(offsetWorldPos, 1.0));
    
    if (abs(sc.w) < 0.001) {
        return 1.0; // Avoid division by zero
    }

    // Perform perspective divide
    vec3 proj = sc.xyz / sc.w;
    vec2 projXY = proj.xy * 0.5 + 0.5;
    projXY.y = 1.0 - projXY.y;

    float normalizedZ = proj.z;

    // Cull out of bounds
    if (any(lessThan(projXY, vec2_splat(0.0))) || any(greaterThan(projXY, vec2_splat(1.0))) || 
        normalizedZ < 0.0 || normalizedZ > 1.0) {
        return 1.0;
    }

    // Determine UV offset based on cascade index
    vec2 uvOffset;
    if (c == 0) uvOffset = vec2(0.0, 0.0);
    else if (c == 1) uvOffset = vec2(0.5, 0.0);
    else if (c == 2) uvOffset = vec2(0.0, 0.5);
    else uvOffset = vec2(0.5, 0.5);
    
    vec2 uv = projXY * 0.5 + uvOffset;

    float cascadeShadowMapSize = 1024.0;

    float baseBias = 0.00005;
    float maxBias = 0.003;

    float cascadeBiasMult = 1.0 + float(c) * 2.0;
    baseBias *= cascadeBiasMult;
    maxBias *= cascadeBiasMult;

    float slopeBias = clamp(1.0 - NdotL, 0.0, 1.0);
    float sunElev = clamp(lightDir.y, 0.0, 1.0);
    float sunsetFactor = smoothstep(0.0, 0.2, 1.0 - sunElev);
    float totalBias = mix(baseBias, maxBias, slopeBias * sunsetFactor);

    float texelSize = 1.0 / cascadeShadowMapSize;
    float shadow = 0.0;
    
    // Sample current cascade
    if (c == 0) {
        shadow = sampleShadowPoisson16(uv, normalizedZ, totalBias, texelSize);
    } else if (c == 1 || c == 2) {
        shadow = sampleShadowPoisson8(uv, normalizedZ, totalBias, texelSize);
    } else {
        shadow = sampleShadow2x2PCF(uv, normalizedZ, totalBias, texelSize);
    }

    // Blend with next cascade only when transitioning
    if (fade > 0.0 && c > 0) {
        // Sample previous cascade (higher resolution)
        vec4 prevSC = mul(u_csmLightViewProj[c - 1], vec4(offsetWorldPos, 1.0));
        vec3 prevProj = prevSC.xyz / prevSC.w;
        vec2 prevProjXY = prevProj.xy * 0.5 + 0.5;
        prevProjXY.y = 1.0 - prevProjXY.y;
        
        vec2 prevUvOffset;
        if (c - 1 == 0) prevUvOffset = vec2(0.0, 0.0);
        else if (c - 1 == 1) prevUvOffset = vec2(0.5, 0.0);
        else prevUvOffset = vec2(0.0, 0.5);
        
        vec2 prevUv = prevProjXY * 0.5 + prevUvOffset;
        
        // Check if previous cascade UV is valid
        if (all(greaterThanEqual(prevProjXY, vec2_splat(0.0))) && all(lessThanEqual(prevProjXY, vec2_splat(1.0)))) {
            float prevShadow;
            if (c - 1 == 0) {
                prevShadow = sampleShadowPoisson16(prevUv, prevProj.z, totalBias, texelSize);
            } else {
                prevShadow = sampleShadowPoisson8(prevUv, prevProj.z, totalBias, texelSize);
            }
            shadow = mix(prevShadow, shadow, fade);
        }
    }

    return mix(0.2, 1.0, 1.0 - shadow);
    //return 1.0 - shadow;
}
