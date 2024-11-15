#version 330 core

out vec4 fragColor;

uniform vec3 lightColor; 
uniform float ambientStrength; 

void main() {

    vec3 rainColor = vec3(1.0, 0.1, 0.3);


    float brightness = ambientStrength + 0.3; 
    rainColor *= brightness;


    fragColor = vec4(rainColor, 0.5); 
}
