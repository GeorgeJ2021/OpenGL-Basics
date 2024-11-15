#version 330 core

layout(location = 0) in vec3 position; 
layout(location = 1) in vec4 color;     

uniform mat4 modelMatrix;   
uniform mat4 viewMatrix;    
uniform mat4 projMatrix;    

out vec4 fragColor;        

void main() {
    fragColor = color; 
    gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(position, 1.0);
}
