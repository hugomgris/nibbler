#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

out vec4 finalColor;

uniform sampler2D texture0;
uniform float strength;

void main() {
    vec4 color = texture(texture0, fragTexCoord);
    
    // Vignette (radial darkening)
    float dist = distance(fragTexCoord, vec2(0.5));
    float vignette = smoothstep(0.8, 0.3, dist * strength);
    
    color.rgb *= vignette;
    
    finalColor = color;
}
