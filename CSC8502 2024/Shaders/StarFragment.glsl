#version 330 core

out vec4 fragColor;

uniform float time;  // Time for twinkling effect

void main() {
    // Compute brightness variation for twinkling effect
    float brightness = 0.5 + 0.5 * sin(time + gl_FragCoord.x * 0.1 + gl_FragCoord.y * 0.1);
    fragColor = vec4(vec3(brightness), 1.0);  // White star with varying brightness
}