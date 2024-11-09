#version 330 core

layout(location = 0) in vec3 position;  // Vertex position

out vec2 fragTexCoord;  // Pass texture coordinates to the fragment shader

void main() {
    // Map the quad's 2D position (from -1 to 1) to a texture coordinate (0 to 1)
    fragTexCoord = position.xy * 0.5 + 0.5;

    // Set the vertex position, projecting the 2D quad onto the screen space
    gl_Position = vec4(position, 1.0);
}
