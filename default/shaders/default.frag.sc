$input v_worldPos, v_worldNormal, v_viewDepth, v_vertexColor
#include <bgfx_shader.sh>
#include <shadow.sh>

uniform vec4 u_lightDir;        // World space direction *from* the light source
uniform vec4 u_baseColor;       // Base color of the material
uniform vec4 u_useVertexColor; // Flag to use vertex color

void main() {
    vec3 normal = normalize(v_worldNormal);
    vec3 lightDir = normalize(u_lightDir.xyz); // Direction from light source to fragment

    vec4 col = mix(u_baseColor, v_vertexColor, u_useVertexColor.x);

    float diffuseIntensity = max(dot(normal, lightDir), 0.0);
    if (lightDir.y < 0.0) {
        // If the light is below the horizon, we consider it to be in shadow
        diffuseIntensity = 0.0;
    }
    
    float shadowFactor = getShadowFactor(v_worldPos, normal, u_lightDir, v_viewDepth);
    float ambientIntensity = 0.20;

    vec3 color = col.xyz * (ambientIntensity + diffuseIntensity * shadowFactor);
    gl_FragColor = vec4(color, col.a);
}
