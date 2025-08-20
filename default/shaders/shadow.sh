SAMPLER2D(s_shadowMap, 3);
uniform vec4 u_csmSplits;
uniform mat4 u_csmLightViewProj[4];
uniform vec4 u_viewPos;
uniform vec4 u_shadowParams;

float getShadowFactor(vec3 worldPos, vec3 worldNormal, vec4 lightDir, float viewDepth) {
    // Simple cascade selection based on distance from camera
    int c = 0;
    if (viewDepth > u_csmSplits.x) c = 1;
    if (viewDepth > u_csmSplits.y) c = 2;
    if (viewDepth > u_csmSplits.z) c = 3;

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

    float lightNear = 10.0;
    float lightFar = u_shadowParams.z + 50.0;
    float normalizedZ = proj.z;

    // Optional debug visualization
    //if (u_shadowParams.w > 0.5) {
    //    // Show what cascade is being used
    //    return float(c) / 3.0;
    //}

    // Check if fragment is outside light frustum
    if (any(lessThan(projXY, vec2_splat(0.0))) || any(greaterThan(projXY, vec2_splat(1.0))) || 
        normalizedZ < 0.0 || normalizedZ > 1.0) {
        return 1.0; // Outside frustum, fully lit
    }

    // Calculate atlas UV coordinates for 2x2 cascade layout
    // Hardcoded because modulo operation is not supported on signed integers (Thanks bgfx)
    vec2 uvOffset;
    if (c == 0) uvOffset = vec2(0.0, 0.0);        // Top-left
    else if (c == 1) uvOffset = vec2(0.5, 0.0);   // Top-right
    else if (c == 2) uvOffset = vec2(0.0, 0.5);   // Bottom-left
    else uvOffset = vec2(0.5, 0.5);               // Bottom-right
    
    vec2 uv = projXY * 0.5 + uvOffset;

    float shadowDepth = texture2D(s_shadowMap, uv).r;

    // Bias
    float NdotL = max(dot(normalize(worldNormal), normalize(lightDir.xyz)), 0.0);
    float slopeBias = 0.001 * (1.0 - NdotL);
    float constantBias = 0.0005;
    float bias = slopeBias + constantBias;

    // Check if fragment is in shadow
    float shadowTest = normalizedZ - bias - shadowDepth;

    // Use step function instead of hard comparison to reduce artifacts
    float shadow = step(0.0, shadowTest);
    
    // Blend between lit and shadowed
    return 1.0 - shadow * 0.7; // 0.3 shadow strength, 1.0 fully lit
}