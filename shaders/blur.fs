#version 330

in vec2 fragTexCoord;
out vec4 finalColor;

uniform sampler2D texture0;
uniform vec2 resolution;
uniform bool horizontal;

// Gaussian blur weights (9-tap)
const float weights[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main() {
    vec2 texelSize = 1.0 / resolution;
    vec3 result = texture(texture0, fragTexCoord).rgb * weights[0];
    
    if (horizontal) {
        for (int i = 1; i < 5; i++) {
            result += texture(texture0, fragTexCoord + vec2(texelSize.x * i, 0.0)).rgb * weights[i];
            result += texture(texture0, fragTexCoord - vec2(texelSize.x * i, 0.0)).rgb * weights[i];
        }
    } else {
        for (int i = 1; i < 5; i++) {
            result += texture(texture0, fragTexCoord + vec2(0.0, texelSize.y * i)).rgb * weights[i];
            result += texture(texture0, fragTexCoord - vec2(0.0, texelSize.y * i)).rgb * weights[i];
        }
    }
    
    finalColor = vec4(result, 1.0);
}
