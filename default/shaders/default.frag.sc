$input v_worldPos, v_worldNormal, v_viewDepth
#include <bgfx_shader.sh>
#include "shadow.sh"

uniform vec4 u_lightDir;        // World space direction *from* the light source
uniform vec4 u_baseColor;       // Base color of the material

void main() {
    vec3 normal = normalize(v_worldNormal);
    vec3 directionToLight = normalize(u_lightDir.xyz); // Vector pointing towards the light

    // Diffuse lighting
    float diffuseIntensity = max(dot(normal, directionToLight), 0.0);

    // Shadow factor
    float shadowFactor = getShadowFactor(v_worldPos, normal, u_lightDir, v_viewDepth);

    // TODO: make this from u_floats
    float ambientIntensity = 0.20f; 

    vec3 color = u_baseColor.rgb * (ambientIntensity + diffuseIntensity * shadowFactor);
    
    gl_FragColor = vec4(color, u_baseColor.a);
}