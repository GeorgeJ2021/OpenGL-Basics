#version 330 core

uniform sampler2D diffuseTex ;
uniform vec3 cameraPos ;
uniform vec4 lightColour ;
uniform vec3 lightPos ;
uniform float lightRadius ;

uniform vec4 ambientLightColor;

in Vertex {
 vec3 colour;
 vec2 texCoord;
 vec3 normal;
 vec3 worldPos;
}IN;

out vec4 fragColour;

void main(void) {
    // Calculate diffuse color from texture
    vec4 diffuse = texture(diffuseTex, IN.texCoord);

    // Lighting calculations
    vec3 incident = normalize(lightPos - IN.worldPos);
    vec3 viewDir = normalize(cameraPos - IN.worldPos);
    vec3 halfDir = normalize(incident + viewDir);

    // Lambertian reflection (diffuse)
    float lambert = max(dot(incident, IN.normal), 0.0f);
    float distance = length(lightPos - IN.worldPos);
    float attenuation = 1.0 - clamp(distance / lightRadius, 0.0, 1.0);

    // Specular reflection
    float specFactor = clamp(dot(halfDir, IN.normal), 0.0, 1.0);
    specFactor = pow(specFactor, 60.0);

    // Calculate diffuse and specular colors
    vec3 surface = diffuse.rgb * lightColour.rgb;
    vec3 diffuseLight = surface * lambert * attenuation;
    vec3 specularLight = (lightColour.rgb * specFactor) * attenuation * 0.33;

    // Calculate ambient light effect
    vec3 ambientLight = ambientLightColor.rgb * diffuse.rgb * 0.2; // Adjust intensity as needed

    // Combine lighting
    fragColour.rgb = ambientLight + diffuseLight + specularLight;
    fragColour.a = diffuse.a; // Use texture's alpha value
}

