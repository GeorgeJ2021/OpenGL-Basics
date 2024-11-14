#version 330 core

layout(location = 0) in vec3 position;  // Position of each star

uniform mat4 projMatrix;
uniform mat4 viewMatrix;

void main() {
    gl_Position = projMatrix * viewMatrix * vec4(position, 1.0);
    gl_PointSize = 2.0;  // Set size of the star points
}
