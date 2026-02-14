#version 330

in vec2 fragTexCoord;
out vec4 finalColor;

uniform sampler2D texture0;
uniform float threshold;

void main() {
    vec4 color = texture(texture0, fragTexCoord);
    
    // Calculate brightness
    float brightness = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
    
    // Extract only bright areas above threshold
    if (brightness > threshold) {
        finalColor = color;
    } else {
        finalColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
}
