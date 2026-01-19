$input v_worldViewRay
#include <bgfx_shader.sh>
#include "common.sh"

uniform vec4 u_lightDir;
uniform vec4 u_skyColorZenith; // Zenith sky color during the day
uniform vec4 u_skyColorMidnight; // Zenith sky color at night
uniform vec4 u_sunColor; // Sunlight color
uniform vec4 u_cameraPos; // Guess

// Ray / sphere intersection in the sphere's local space.
// r0 = ray origin, rD = ray direction (normalized), radius = sphere radius.
// Returns the nearest positive hit distance along the ray, or -1 if there is no hit.
float raySphereIntersect(vec3 r0, vec3 rD, float radius)
{
    // Quadratic: |r0 + t*rD|^2 = radius^2
    // With normalized rD, this simplifies to:
    // t^2 + 2*b*t + c = 0, where b = dot(r0, rD), c = dot(r0,r0) - radius^2
    float b = dot(r0, rD);
    float c = dot(r0, r0) - radius * radius;
    float h = b*b - c; // discriminant (must be >= 0 for real intersections)

    float hSqrt = sqrt(h);
    float t0 = -b - hSqrt;
    float t1 = -b + hSqrt;

    // Logic:
    // If t0 > 0, we are outside hitting the front -> Return t0
    // If t0 < 0, we are inside hitting the shell -> Return t1
    if (t0 >= 0.0) return t0;
    if (t1 >= 0.0) return t1;

    return -1.0;
}

// https://www.scratchapixel.com/lessons/procedural-generation-virtual-worlds/simulating-sky/simulating-colors-of-the-sky.html
// Phase function controls how the suns halo spreads
// g = 0 (isotropic), g = 1 (full forward scatter), g = 0.76 (earth atmosphere)
float getMiePhase(float cosTheta, float g) {
    // Henyey-Greenstein-like phase (normalized) for aerosol / Mie scattering.
    // Larger g pushes more energy toward forward scattering (glowy sun halo).
    float g2 = g * g;
    return 3.0 / (8.0 * 3.14159265) * ((1.0 - g2) * (1.0 + cosTheta * cosTheta)) /
    ((2.0 + g2) * pow(abs(1.0 + g2 - 2.0 * g * cosTheta), 1.5));
}

// Scuffed Rayleigh approximation for sky color
float getRayleighPhase(float cosTheta) {
    // Rayleigh phase is symmetric and favors forward/back equally.
    // This gives the familiar "bright around the sun and opposite side" feel,
    // but much less concentrated than Mie.
    return 3.0 / (16.0 * 3.14159265) * (1.0 + cosTheta * cosTheta);
}

void main() {
    // Radii are in kilometers (ish). Camera is also converted to km below.
    // We treat the planet as a sphere centered below the world origin so Y=0 is "ground level".
    const float R_PLANET = 6360.0;
    const float R_ATMO = 6420.0; // Planet + 60km

    // Rayleigh scattering coefficients (RGB wavelength dependence: blue scatters more).
    // Bigger value => more scattering (i.e. more "sky color" contribution).
    const vec3 BETA_R = vec3(5.5e-3, 13.0e-3, 22.4e-3);
    const float BETA_M = 21.0e-3;

    // Scale heights (km): how quickly density falls off with altitude.
    // Rayleigh: higher scale height (molecules), Mie: lower (aerosols near ground).
    const float H_R = 7.994; //Rayleigh height
    const float H_M = 1.200; //Mie height.

    // v_worldViewRay comes from the fullscreen triangle vertex shader: worldPos - camPos.
    // This is effectively a ray direction through the pixel into the scene.
    vec3 viewDir = normalize(v_worldViewRay.xyz);

    // u_lightDir is treated as "direction to sun" in world space.
    vec3 lightDirToSun = normalize(u_lightDir.xyz);

    // Camera position from inverse view matrix, scaled to match the km units above.
    vec3 camPos = u_invView[3].xyz * 0.001;

    // Planet center is shifted down so that world origin sits on the surface.
    vec3 planetCenter = vec3(0.0, -R_PLANET, 0.0);

    // Clamp rays below the horizon so we don't march into the planet.
    // This is a cheap fix: force any downward view rays to be horizontal.
    vec3 marchDir = viewDir;
    if (marchDir.y < 0.0) {
        marchDir.y = 0.0;
        marchDir = normalize(marchDir);
    }

    // Find how far the view ray travels before exiting the atmosphere.
    // We intersect the atmosphere sphere with a ray starting at the camera, in planet-centered space.
    float distToTop = raySphereIntersect(camPos - planetCenter, marchDir, R_ATMO);

    // If somehow we miss the atmosphere (bad setup), fall back to a small march distance.
    if (distToTop < 0.0) distToTop = 10.0;

    const int iSteps = 10; //Quality (view ray marching steps)
    const int jSteps = 4; //Light steps (secondary march toward the sun)

    float stepSize = distToTop / float(iSteps);
    float currentPosOnRay = 0.0;

    // Integrated scattering contribution along the view ray (separated by component).
    vec3 totalRayleigh = vec3_splat(0.0);
    vec3 totalMie = vec3_splat(0.0);

    // Optical depth accumulators along the view ray.
    // Think of this as "how much atmosphere we've traveled through so far".
    float optDepthR = 0.0;
    float optDepthM = 0.0;

    // Angle between view ray and light direction:
    // mu=1 => looking at sun, mu=-1 => looking away.
    float mu = dot(viewDir, lightDirToSun);
    float phaseR = getRayleighPhase(mu);
    float phaseM = getMiePhase(mu, 0.76); //g=0.76 is roughly earthy

    UNROLL
    for (int i = 0; i < iSteps; i++) {
        // Sample at the midpoint of the current segment to reduce bias (midpoint rule).
        vec3 samplePos = camPos + viewDir * (currentPosOnRay + stepSize * 0.5);

        // Height above ground in km.
        float height = length(samplePos - planetCenter) - R_PLANET;

        // Local density approximation: exp(-height / scaleHeight).
        // Multiply by stepSize to accumulate optical depth for this segment.
        float hr = exp(-max(height, 0.0) / H_R) * stepSize;
        float hm = exp(-max(height, 0.0) / H_M) * stepSize;

        optDepthR += hr;
        optDepthM += hm;

        // For this view-ray sample, estimate how much atmosphere the sun light passes through
        // before reaching this point (secondary ray marching toward the sun).
        float distToSun = raySphereIntersect(samplePos - planetCenter, lightDirToSun, R_ATMO);
        float stepSizeSun = distToSun / float(jSteps);

        float sunOptDepthR = 0.0;
        float sunOptDepthM = 0.0;
        if (distToSun > 0.0) {
            UNROLL
            for (int j = 0; j < jSteps; j++) {
                vec3 sunSamplePos = samplePos + lightDirToSun * (stepSizeSun * (float(j) + 0.5));
                float sunHeight = length(sunSamplePos - planetCenter) - R_PLANET;

                // Same density model along the sun ray.
                sunOptDepthR += exp(-max(sunHeight, 0.0) / H_R) * stepSizeSun;
                sunOptDepthM += exp(-max(sunHeight, 0.0) / H_M) * stepSizeSun;
            }
        } else {
            // No path to the sun through the atmosphere => treat as fully occluded.
            // Large optical depth makes attenuation ~ 0.
            sunOptDepthM = 100.0;
            sunOptDepthR = 100.0;
        }

        // Beer-Lambert attenuation: exp(-sigma * opticalDepth).
        // We clamp the exponent to avoid underflow/NaNs on some platforms.
        vec3 atten = exp(-clamp(
        BETA_R * (optDepthR + sunOptDepthR)
        + BETA_M * 1.1 * (optDepthM + sunOptDepthM),
        vec3_splat(0.0),
        vec3_splat(50.0)
        ));

        // Accumulate in-scattered light for this segment.
        // Intuition: (local density) * (how much sun light makes it here).
        totalRayleigh += atten * hr;
        totalMie += atten * hm;

        currentPosOnRay += stepSize;
    }

    // Convert integrated densities into RGB radiance via scattering coefficients and phase functions.
    vec3 colorR = totalRayleigh * BETA_R * phaseR;
    vec3 colorM = totalMie * BETA_M * phaseM;

    // Overall artistic scaling; physically you'd model solar irradiance etc.
    float sunIntensity = 20.0;
    vec3 finalColor = (colorR + colorM) * sunIntensity;
    finalColor *= u_sunColor.rgb;

    // Cheap ground fade: if looking below horizon, blend toward a darker "ground" tint
    // so the sky doesn't look like it's continuing through the planet.
    float groundFactor = smoothstep(-0.25, 0.05, viewDir.y);
    vec3 groundColor = finalColor * 0.2;
    if (viewDir.y < 0.0) {
        finalColor = mix(groundColor, finalColor, groundFactor);
    }

    gl_FragData[0] = vec4(finalColor, 1.0);
    gl_FragData[1] = vec4(0.0, 0.0, 0.0, 1.0); // Dummy normal
}