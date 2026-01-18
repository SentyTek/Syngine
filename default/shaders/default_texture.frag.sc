$input v_uvMacro v_uvDetail v_normal v_tangent v_worldPos v_viewDepth
#include <bgfx_shader.sh>
#include "shadow.sh"
#include "common.sh"

//texture slots
SAMPLER2D(s_albedo, 0);
SAMPLER2D(s_normalMap, 1);
SAMPLER2D(s_heightMap, 2);

//lighting + material params
uniform vec4 u_lightDir; //worldspace sun dir
uniform vec4 u_floats;   //[x = heightmap scale, y = mix factor, z = ambient, w = detail tile scale]
uniform vec4 u_skyColor; //hemisphere ambient sky color
uniform vec4 u_sunColor; //sunlight color
uniform vec4 u_horizonColor; // same as u_scatterColor in sky shader (horizon scatter color)

void main() {
    vec3 lightDirToSun = normalize(u_lightDir.xyz);
    float sunElevation = lightDirToSun.y;

    // Get base terrain normal (captures large-scale terrain slopes from vertex normals)
    vec3 vN = normalize(v_normal);
    
    // Normal mapping from normal map (micro detail)
    vec3 nmap = texture2D(s_normalMap, v_uvDetail).xyz * 2.0 - 1.0;
    vec3 T = normalize(v_tangent.xyz);
    vec3 B = cross(vN, T) * v_tangent.w; // Calculate bitangent
    vec3 Nmicro = normalize(T*nmap.x + B*nmap.y + vN*nmap.z);
    
    // Mix base terrain normal with detail normal map
    // u_floats.y controls how much detail normal mapping is applied
    vec3 N = normalize(mix(vN, Nmicro, u_floats.y));

    // Hemisphere ambient (http://richardssoftware.net/home/Post/58)
    // Instead of flat ambient, mix sky and ground colors based on normal Y
    float hemiMix = N.y * 0.5 + 0.5;
    vec3 skyColor = u_skyColor.xyz;
    vec4 albedo = texture2D(s_albedo, v_uvDetail);

    // Rotate lightDirToSun by 90 degrees around the X axis for micro shadowing
    vec3 rotatedLightDir = vec3(lightDirToSun.x, -lightDirToSun.z, lightDirToSun.y);

    // Lighting is direct sun
    float NdotL = smoothstep(-0.12, 0.15, max(dot(N, rotatedLightDir), 0.0)); //Smoothstep to extend light over horizon
    vec3 ambient = mix(albedo.rgb, skyColor, hemiMix) * u_floats.z * clamp(NdotL, 0.1, 1.0); // Ambient scaled by NdotL for better blending at low sun angles

    // Micro shadowing from normal map
    // Softer attenuation for grazing angles
    float microShadow = clamp(dot(Nmicro, rotatedLightDir) * 2.0 + 0.3, 0.0, 1.0);
    float shadow = getShadowFactor(v_worldPos, vN, N, u_lightDir, v_viewDepth);
    shadow = mix(1.0, shadow, smoothstep(0.1, 0.3, sunElevation)); // Fade shadows to full light (1.0) at low sun angles

    float sunIntensity = smoothstep(-0.12, 0.15, sunElevation);
    vec3 directLight = u_sunColor.xyz * sunIntensity * NdotL * shadow * microShadow;

    // Fake first-bounce global illumination
    vec3 bounce = u_sunColor.xyz * sunIntensity * 0.2 * clamp(dot(N, rotatedLightDir), 0.0, 1.0);

    // Calculate view direction
    vec3 viewDir = normalize(u_viewPos.xyz - v_worldPos);

    // Specular. Using Schlick's approximation for fresnel
    vec3 F0 = vec3_splat(0.02); // Lower reflectance for rough ground
    vec3 halfVec = normalize(lightDirToSun + viewDir);
    float NdotH = max(dot(N, halfVec), 0.0);

    // Blinn-Phong specular
    float specPower = 64.0; // Higher power = tighter, subtler specular
    float specStrength = pow(NdotH, specPower) * microShadow * shadow * 0.15; // Much weaker specular for ground

    vec3 fresnel = fresnelSchlick(max(dot(N, viewDir), 0.0), F0);

    // Fog
    float fogDist = length(v_worldPos);
    float fogFactor = 1.0 - exp(-fogDist * 0.0005);

    // Combine
    vec3 finalColor = albedo.rgb * (ambient + directLight + bounce) + (directLight * specStrength * fresnel);
    finalColor = mix(finalColor, u_horizonColor.xyz, fogFactor);
    
    // Gamma correction (linear to sRGB) (Tonemapping done in post now)
    //finalColor = applyGammaCorrection(finalColor, 2.2);
    
    gl_FragData[0] = vec4(finalColor, albedo.a);
    //gl_FragData[0] = vec4(directLight, 1.0); //debug: sun elevation
    gl_FragData[1] = vec4(N * 0.5 + 0.5, 1.0); //normal output (world space, encoded to [0,1])
}
