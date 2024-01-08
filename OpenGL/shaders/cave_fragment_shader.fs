#version 330 core
out vec4 FragColor;

in vec3 normal; // Received from the vertex shader
in vec2 TexCoord; // Texture coordinates passed from the vertex shader
in vec3 FragPos; // World position passed from the vertex shader

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float blendFactor; // Blend factor for textures

uniform vec3 objectColor; // Color of the object
uniform vec3 lightColor;  // Color of the first light
uniform vec3 lightDir;    // Direction of the first light
uniform vec3 ambientStrength; // Strength of the ambient lighting

uniform vec3 secondLightDir;    // Second light direction
uniform vec3 secondLightColor;  // Second light color
uniform vec3 secondAmbientStrength; // Second light ambient strength

uniform vec3 viewPos; // Camera position for specular calculation

// Torch light properties
uniform vec3 torchPos; // Position of the torch
uniform vec3 torchLightColor; // Color of the torch light (e.g., orange)

// New uniform for the camera's Y position
uniform float cameraY;  

// Biome change threshold
const float biomeChangeYLevel = 20.0;

void main()
{
    // Sample the texture colors
    vec4 texColor1 = texture(texture1, TexCoord);
    vec4 texColor2 = texture(texture2, TexCoord);

    // Mix the two textures based on the blend factor
    vec4 finalColor = mix(texColor1, texColor2, blendFactor);

    // Adjust the light color and strength to be less bright
    vec3 ambient = ambientStrength * lightColor;
    vec3 ambient2 = secondAmbientStrength * secondLightColor;

    // Adjust ambient light based on the biome
    if (cameraY < biomeChangeYLevel) {
        ambient = vec3(0.2, 0.2, 0.5);  // Example: darker, bluish tone
        ambient2 = vec3(0.2, 0.2, 0.5);
    }

    // Normals
    vec3 norm = normalize(normal);

    // Calculate lighting from the main light sources
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    float diff2 = max(dot(norm, secondLightDir), 0.0);
    vec3 diffuse2 = diff2 * secondLightColor;

    // Calculate torch light (Point light with attenuation)
    float distance = length(torchPos - FragPos);
    float constant = 1.0;
    float linear = 0.09;
    float quadratic = 0.032;
    float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));

    vec3 torchDir = normalize(torchPos - FragPos);
    float torchDiff = max(dot(norm, torchDir), 0.0);
    vec3 torchDiffuse = torchDiff * torchLightColor * attenuation;

    // Add torch light to the scene
    vec3 result = (ambient + diffuse + ambient2 + diffuse2 + torchDiffuse) * finalColor.rgb;
    result = mix(result, objectColor, 0.2); // Blend with object color

    FragColor = vec4(result, finalColor.a); 
}
