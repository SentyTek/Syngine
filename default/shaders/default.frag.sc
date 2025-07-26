$input v_worldNormal
#include <bgfx_shader.sh>

uniform vec4 u_lightDir;        // World space direction *from* the light source
uniform vec4 u_baseColor;       // Base color of the material

void main() {
    vec3 normal = normalize(v_worldNormal);
    vec3 directionToLight = normalize(-u_lightDir.xyz); // Vector pointing towards the light

    // Diffuse lighting
    float diffuseIntensity = max(dot(normal, directionToLight), 0.0);
    
    // TODO: make this from u_floats
    float ambientIntensity = 0.20f; 

    vec3 diffuseColorContribution = u_baseColor.rgb * diffuseIntensity;
    vec3 ambientColorContribution = u_baseColor.rgb * ambientIntensity;

    vec3 finalColor = clamp(diffuseColorContribution + ambientColorContribution, 0.0, 1.0);
    
    gl_FragColor = vec4(finalColor, u_baseColor.a);
}