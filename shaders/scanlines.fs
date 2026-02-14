#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

out vec4 finalColor;

uniform sampler2D texture0;
uniform vec2 resolution;
uniform float intensity;

void main() {
    vec4 color = texture(texture0, fragTexCoord);
    
    // Scanlines
    float scanline = sin(fragTexCoord.y * resolution.y * 2.0) * intensity;
    color.rgb -= scanline;
    
    finalColor = color;
}
