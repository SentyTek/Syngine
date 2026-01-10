$input v_worldPos, v_worldNormal, v_viewDepth, v_vertexColor
#include <bgfx_shader.sh>
#include "shadow.sh"
#include "common.sh"

uniform vec4 u_lightDir;        // World space direction *from* the light source
uniform vec4 u_baseColor;       // Base color of the material
uniform vec4 u_useVertexColor;  // Flag to use vertex color
uniform vec4 u_floats;          // [x = unused, y = unused, z = ambient, w = unused]
uniform vec4 u_skyColor;        // hemisphere ambient sky color
uniform vec4 u_sunColor;        // sunlight color
uniform vec4 u_horizonColor;    // fog/horizon color

void main() {
    vec3 normal = normalize(v_worldNormal);
    vec3 lightDirToSun = normalize(u_lightDir.xyz);
    float sunElevation = lightDirToSun.y;

    vec4 col = mix(u_baseColor, v_vertexColor, u_useVertexColor.x);

    // Hemisphere ambient (http://richardssoftware.net/home/Post/58)
    // Instead of flat ambient, mix sky and ground colors based on normal Y
    float hemiMix = normal.y * 0.5 + 0.5;
    vec3 skyColor = u_skyColor.xyz;
    vec3 ambient = mix(col.rgb, skyColor, hemiMix) * u_floats.z;

    // Lighting is direct sun
    float NdotL = max(dot(normal, lightDirToSun), 0.0);
    
    float shadowFactor = getShadowFactor(v_worldPos, vec3(0.0, 1.0, 0.0), normal, u_lightDir, v_viewDepth);
    
    float sunIntensity = smoothstep(-0.1, 0.1, sunElevation);
    vec3 directLight = u_sunColor.xyz * sunIntensity * NdotL * shadowFactor;

    // Fake first-bounce global illumination
    vec3 bounce = u_sunColor.xyz * sunIntensity * 0.2 * clamp(dot(normal, -lightDirToSun), 0.0, 1.0);

    // Calculate view direction
    vec3 viewDir = normalize(u_viewPos.xyz - v_worldPos);

    // Specular. Using Schlick's approximation for fresnel
    vec3 F0 = vec3_splat(0.04); // roughly linear
    vec3 halfVec = normalize(lightDirToSun - viewDir);
    float NdotH = max(dot(normal, halfVec), 0.0);

    // Blinn-Phong specular
    float specPower = 32.0; // Rough surface
    float specStrength = pow(NdotH, specPower) * shadowFactor; // modulated by shadowing

    vec3 fresnel = fresnelSchlick(max(dot(normal, viewDir), 0.0), F0);

    // Fog
    float fogDist = length(v_worldPos);
    float fogFactor = 1.0 - exp(-fogDist * 0.0005);

    // Combine
    vec3 finalColor = col.rgb * (ambient + directLight + bounce) + (directLight * specStrength * fresnel);
    finalColor = mix(finalColor, u_horizonColor.xyz, fogFactor);

    // Tone mapping
    finalColor = ACESFilm(finalColor);
    
    gl_FragColor = vec4(finalColor, col.a);
}
