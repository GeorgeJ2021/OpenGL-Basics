#version 330 core

in vec2 fragTexCoord;
out vec4 fragColor;

uniform float flashIntensity;

void main() {
    vec3 flashColor = vec3(1.0, 1.0, 1.0);    // White color for flash
    fragColor = vec4(flashColor * flashIntensity, flashIntensity);
}