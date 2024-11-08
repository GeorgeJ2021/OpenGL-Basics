# version 330 core

in vec2 texCoord;
uniform sampler2D diffuseTex;
uniform vec3 ambientLight; // Moonlight color passed as a uniform

out vec4 fragColor;

void main() {
    vec4 baseColor = texture(diffuseTex, texCoord); // Base texture from G-buffer
    vec3 ambientComponent = ambientLight * baseColor.rgb; // Apply ambient light

    fragColor = vec4(ambientComponent, 1.0); // Final color with only ambient
}