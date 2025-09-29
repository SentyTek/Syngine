$input v_uvMacro v_uvDetail v_normal v_tangent v_worldPos v_viewDepth
#include <bgfx_shader.sh>
#include "shadow.sh"

//texture slots
SAMPLER2D(s_albedo, 0);
SAMPLER2D(s_normalMap, 1);
SAMPLER2D(s_heightMap, 2);

//lighting + material params
uniform vec4 u_lightDir;        //worldspace sun dir
uniform vec4 u_floats;          //[x = heightmap scale, y = mix factor, z = ambient, w = detail tile scale]

void main() {
    float SUN_ELEV_TWILIGHT_START = -0.1;
    float SUN_ELEV_DEEP_TWILIGHT_END = -0.3;

    vec3 lightDirToSun = normalize(u_lightDir.xyz);
    float sunElevation = lightDirToSun.y;

    //macro normal from heightmap
    float du    = 1.0/float(textureSize(s_heightMap, 0).x); //one texel in UV
    float hl    = texture2D(s_heightMap, v_uvMacro + vec2(-du, 0.0)).r;
    float hr    = texture2D(s_heightMap, v_uvMacro + vec2(du, 0.0)).r;
    float hd    = texture2D(s_heightMap, v_uvMacro + vec2(0.0, -du)).r;
    float hu    = texture2D(s_heightMap, v_uvMacro + vec2(0.0, du)).r;
    vec3 dPdu   = vec3(du, 0, (hr - hl) * u_floats.x);
    vec3 dPdv   = vec3(0, du, (hu - hd) * u_floats.x);
    vec3 Nmacro = normalize(cross(dPdu, dPdv));

    //micro normal from normal map
    vec3 nmap   = texture2D(s_normalMap, v_uvDetail).xyz*2.0 - 1.0;
    float normalStrength = 1.0;
    nmap.xy     = mix(vec2(0.0, 0.0), nmap.xy, normalStrength);
    nmap        = normalize(nmap);
    vec3 T      = normalize(v_tangent.xyz);
    vec3 v_normalNormed = normalize(v_normal);
    vec3 B      = cross(v_normalNormed, T) * v_tangent.w;
    vec3 Nmicro = normalize(T*nmap.x + B*nmap.y + v_normalNormed*nmap.z);

    //blend
    vec3 N      = normalize( mix(Nmacro, Nmicro, u_floats.y) );

    // use less detailed normal for shadows to reduce acne
    vec3 shadowNormal = normalize( mix(Nmacro, Nmicro, u_floats.y * 0.3) );

    //sun based lighting
    float NdotL             = max(dot(N, lightDirToSun), 0.0);
    float sunLightStrength  = smoothstep(SUN_ELEV_TWILIGHT_START, 0.05, sunElevation);
    float minNightAmbient   = 0.1;
    float currentAmbient    = mix(minNightAmbient, u_floats.z, smoothstep(SUN_ELEV_DEEP_TWILIGHT_END, SUN_ELEV_TWILIGHT_START, sunElevation));

    // Shadow factor
    float shadow = getShadowFactor(v_worldPos, N, u_lightDir, v_viewDepth);

    float shadowMix = Nmicro.y / 3.0;
    float shadowFactor = mix(1.0, shadow, shadowMix);
    float finalLight = currentAmbient + NdotL * sunLightStrength * shadow;
    //finalize and mix
    vec4 base       = texture2D(s_albedo, v_uvDetail);
    gl_FragColor    = vec4(base.rgb * finalLight, base.a);
}
