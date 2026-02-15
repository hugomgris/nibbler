#version 330

// Input from vertex shader
in vec2 fragTexCoord;
in vec4 fragColor;

// Output
out vec4 finalColor;

// Uniforms
uniform sampler2D texture0;      // Original scene
uniform sampler2D bloomTexture;  // Blurred bright areas
uniform vec2 resolution;
uniform float time;

// CRT Effect parameters
uniform float scanlineIntensity;
uniform float curvatureAmount;
uniform float vignetteStrength;
uniform float chromaticAberration;
uniform float grainAmount;
uniform float bloomIntensity;

// Barrel distortion
vec2 curveUV(vec2 uv) {
    if (curvatureAmount <= 0.0) return uv;
    
    vec2 centered = uv * 2.0 - 1.0;
    float r2 = dot(centered, centered);
    float distortion = 1.0 + curvatureAmount * r2;
    centered *= distortion;
    return centered * 0.5 + 0.5;
}

// Scanlines effect
float scanlines(vec2 uv) {
    float scanline = sin(uv.y * resolution.y * 2.0) * scanlineIntensity;
    return 1.0 - scanline;
}

// Vignette effect
float vignette(vec2 uv) {
    float dist = distance(uv, vec2(0.5));
    return smoothstep(0.8, 0.3, dist * vignetteStrength);
}

// noise
float noise(vec2 co) {
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233)) + time) * 43758.5453);
}

// RGB phosphor grid
vec3 shadowMask(vec2 uv) {
    vec3 mask = vec3(1.0);
    float x = fract(uv.x * resolution.x / 3.0);
    
    if (x < 0.333) {
        mask.r *= 1.1;
        mask.gb *= 0.9;
    } else if (x < 0.666) {
        mask.g *= 1.1;
        mask.rb *= 0.9;
    } else {
        mask.b *= 1.1;
        mask.rg *= 0.9;
    }
    
    return mask;
}

void main() {
    // Apply CRT curvature
    vec2 curvedUV = curveUV(fragTexCoord);
    
    // Check if outside screen bounds (black bars)
    if (curvedUV.x < 0.0 || curvedUV.x > 1.0 || curvedUV.y < 0.0 || curvedUV.y > 1.0) {
        finalColor = vec4(0.0, 0.0, 0.0, 1.0);
        return;
    }
    
    // Sample original scene with chromatic aberration
    vec3 color;
    if (chromaticAberration > 0.0) {
        float aberration = chromaticAberration * (distance(fragTexCoord, vec2(0.5)) * 2.0);
        vec2 offsetR = vec2(aberration, 0.0);
        vec2 offsetB = vec2(-aberration, 0.0);
        
        color.r = texture(texture0, curvedUV + offsetR).r;
        color.g = texture(texture0, curvedUV).g;
        color.b = texture(texture0, curvedUV + offsetB).b;
    } else {
        color = texture(texture0, curvedUV).rgb;
    }
    
    // Add bloom
    vec3 bloom = texture(bloomTexture, curvedUV).rgb;
    color += bloom * bloomIntensity;
    
    // Apply scanlines
    color *= scanlines(curvedUV);
    
    // Apply phosphor mask (subtle)
    color *= mix(vec3(1.0), shadowMask(curvedUV), 0.3);
    
    // Apply vignette
    color *= vignette(fragTexCoord);
    
    // Add film grain
    if (grainAmount > 0.0) {
        float grain = noise(curvedUV * resolution) * grainAmount;
        color += grain;
    }
    
    // Slight color warmth (CRT phosphor tint)
    color *= vec3(1.0, 0.98, 0.95);
    
    finalColor = vec4(color, 1.0);
}
