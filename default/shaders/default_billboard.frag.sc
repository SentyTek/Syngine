$input v_texcoord0, v_worldPos
#include <bgfx_shader.sh>
#include <shadow.sh>

SAMPLER2D(s_albedo, 0);

uniform vec4 u_lightDir;
uniform vec4 u_billboard_lighting; // x: receiveSunLight (bool), y = receiveShadows (bool)

void main() {
    // Sample the albedo texture
    vec4 base = texture2D(s_albedo, v_texcoord0);

    // Alpha Test: Discard the fragment if its alpha is below a threshold.
    // This allows for proper transparency handling in billboards.
    if (base.a < 0.1) {
        discard;
    }

    vec3 finalColor = base.rgb;

    // Apply sun lighting if enabled
    if (u_billboard_lighting.x > 0.5) {
        // Simple diffuse lighting assuming billboard normal faces camera
        vec3 lightDir = normalize(u_lightDir.xyz);
        float diffuse = max(0.0, lightDir.y); // Simplified lighting based on sun elevation
        float ambient = 0.2;
        
        float lightFactor = ambient + diffuse * 0.8;
        float nightFactor = clamp(smoothstep(-0.05, 0.05, lightDir.y), 0.2, 1.0);
        lightFactor *= nightFactor;
        
        // Apply shadow if enabled
        if (u_billboard_lighting.y > 0.5) {
            // Calculate view depth (distance from camera)
            float viewDepth = length(v_worldPos - u_viewPos.xyz);
            vec3 fakeNormal = vec3(0.0, 1.0, 0.0); // Assume upward facing normal
            float shadowFactor = getShadowFactor(v_worldPos, vec3(0.0, 1.0, 0.0), fakeNormal, u_lightDir, viewDepth);
            lightFactor *= shadowFactor;
        }
        
        finalColor *= lightFactor;
    }

    gl_FragData[0] = vec4(finalColor, base.a);
    gl_FragData[1] = vec4(0.0, 0.0, 1.0, 1.0);
}
