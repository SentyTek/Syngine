$input v_worldViewRay
#include <bgfx_shader.sh>

uniform vec4 u_lightDir;
uniform vec4 u_skyColorZenith; // Zenith sky color during the day
uniform vec4 u_skyColorMidnight; // Zenith sky color at night
uniform vec4 u_sunColor; // Sunlight color
uniform vec4 u_scatterColor; // Horizon scatter color

// https://www.scratchapixel.com/lessons/procedural-generation-virtual-worlds/simulating-sky/simulating-colors-of-the-sky.html
// Phase function controls how the suns halo spreads
// g = 0 (isotropic), g = 1 (full forward scatter), g = 0.76 (earth atmosphere)
float getMiePhase(float cosTheta, float g) {
    float g2 = g * g;
    return (1.0 - g2) / pow (abs(1.0 + g2 - 2.0 * g * cosTheta), 1.5) * 1.5 / (4.0 * 3.14159265);
}

// Scuffed Rayleigh approximation for sky color
float getRayleighPhase(float cosTheta) {
    return 3.0 / (16.0 * 3.14159265) * (1.0 + cosTheta * cosTheta);
}

void main() {
    vec3 viewDir = normalize(v_worldViewRay.xyz);
    vec3 lightDirToSun = normalize(u_lightDir.xyz);

    float sunElevation = lightDirToSun.y;
    float viewElevation = viewDir.y;
    float cosTheta = dot(viewDir, lightDirToSun);

    float SUN_ELEVATION_TWILIGHT = -0.1;
    float dayIntensity = smoothstep(SUN_ELEVATION_TWILIGHT - 0.1, 0.1, sunElevation);

    // Rayleigh scattering. Approximate optical depth based on 1.0 / viewElevation
    // Atmospheric "thickness"
    float zenithCurve = 1.0 - pow(max(viewElevation, 0.0), 0.35); // 0.35 controls horizon haze thickness
    vec3 zenithColor = mix(u_skyColorMidnight.xyz, u_skyColorZenith.xyz, dayIntensity);

    // Horizon color
    float sunsetFactor = 1.0 - smoothstep(0.0, 0.5, sunElevation);
    vec3 horizonColor = mix(vec3(0.9, 0.95, 1.0), u_scatterColor.rgb, sunsetFactor);
    horizonColor *= dayIntensity; // No horizon color at night

    // Combined zenith and horizon based on view elevation
    vec3 skyGradient = mix(zenithColor, horizonColor, zenithCurve);

    // Sun halo
    // Mie scattering creates the bright halo around the sun
    float sunDisk = getMiePhase(cosTheta, 0.9995); // Sharp disk
    float sunHalo = getMiePhase(cosTheta, 0.76); // Wider halo
    
    // Energy conservation or something? dampen at night
    vec3 sunLight = u_sunColor.rgb * dayIntensity;

    // Turn sun red at low elevations
    float extinction = exp(-1.0 / (max(sunElevation, 0.1) * 10.0)); // Controls how quickly sun reddens
    vec3 sunAbsorbed = mix(u_scatterColor.rgb, sunLight, clamp(extinction * 4.0, 0.0, 1.0));

    vec3 mieScatter = sunAbsorbed * (sunDisk * 500.0 + sunHalo * 2.0); // Factors can be tweaked for intensity

    // Mask mie scatter so sun doesn't appear at night
    mieScatter *= smoothstep(-0.05, 0.05, viewElevation + 0.05);

    // Composition
    vec3 finalSky = skyGradient + mieScatter;
    
    finalSky = finalSky / (finalSky + vec3_splat(1.0)); // Tone mapping
    finalSky = pow(abs(finalSky), vec3_splat(1.0/2.2)); // Gamma correction
    gl_FragColor = vec4(finalSky, 1.0);
}