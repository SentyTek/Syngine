$input v_worldViewRay
#include <bgfx_shader.sh>

uniform vec4 u_lightDir;
uniform vec4 u_skyColorDay;
uniform vec4 u_skyColorNight;
uniform vec4 u_sunColorDay;
uniform vec4 u_sunColorRise;

void main() {
    float SUN_ELEVATION_DEEP_TWILIGHT_END = -0.309;

    vec3 lightDir = normalize(-u_lightDir.xyz);

    float sunElevation = lightDir.y;
    float dayIntensity = smoothstep(SUN_ELEVATION_DEEP_TWILIGHT_END, 0.2, sunElevation);
    vec3 skyColor = mix(u_skyColorNight.rgb, u_skyColorDay.rgb, dayIntensity);

    if (sunElevation > SUN_ELEVATION_DEEP_TWILIGHT_END) { //only draw sun if its near or above the horizon
        float cosAngle = dot(normalize(v_worldViewRay), lightDir);
        float sunColorIntensity = smoothstep(SUN_ELEVATION_DEEP_TWILIGHT_END + 0.2, 0.2, sunElevation);
        vec3 sunColor = mix(u_sunColorRise.rgb, u_sunColorDay.rgb, sunColorIntensity);

        //sun disk - adjust 0.999 to change size. closer to 1 equals smaller sun
        //cos(0.5d) ~ 0.99996
        //cos(1d) ~ 0.9998
        //cos(2d) ~ 0.9993
        float sunDiskThreshold = 0.999; //2.5d
        float sunEdgeSoftness = 0.0001;

        //sun core
        float sunCoreIntensity = smoothstep(sunDiskThreshold, sunDiskThreshold + sunEdgeSoftness, cosAngle);

        //sun glow
        float sunGlowRadius = 0.005;
        float sunGlowIntensity = smoothstep(sunDiskThreshold - sunGlowRadius, sunDiskThreshold, cosAngle);

        //compile and mix
        vec3 glowColorContrib = sunColor.rgb * 0.5 + skyColor * 0.5;
        skyColor = mix(skyColor, glowColorContrib, sunGlowIntensity);
        skyColor = mix(skyColor, sunColor.rgb, sunCoreIntensity);
    }

    gl_FragColor = vec4(skyColor.rgb, 1.0);
    //gl_FragColor = vec4(sunElevation, sunElevation, sunElevation, 1.0);
}