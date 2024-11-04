#version 330 core

uniform sampler2D diffuseTex;
uniform samplerCube cubeTex;
uniform sampler2D bumpTex; 

uniform vec3 cameraPos;

in Vertex {
    vec4 colour;
    vec2 texCoord;
    vec3 normal;
    vec3 worldPos;
} IN;

out vec4 fragColour;

void main(void) {
    
    vec4 diffuse = texture(diffuseTex, IN.texCoord);

    
    vec3 viewDir = normalize(cameraPos - IN.worldPos);

    
    vec2 bumpScale = vec2(10.0); // Scale factor for tiling bump map
    vec3 bumpSample = texture(bumpTex, IN.texCoord * bumpScale).xyz;

    
    bumpSample = bumpSample * 2.0 - 1.0;

    
    vec3 perturbedNormal = normalize(IN.normal + bumpSample * 0.1); // 0.1 is the strength of the bump effect

    
    vec3 reflectDir = reflect(-viewDir, perturbedNormal);

    
    vec4 reflectTex = texture(cubeTex, reflectDir);

    
    fragColour = reflectTex + (diffuse * 0.25f);
}
