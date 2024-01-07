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

    // Ambient light calculation for each light source
    vec3 ambient = ambientStrength * lightColor;
    vec3 ambient2 = secondAmbientStrength * secondLightColor;

    // Adjust ambient light based on the biome
    if (cameraY < biomeChangeYLevel) {
        ambient = vec3(0.2, 0.2, 0.5);  // Example: darker, bluish tone
        ambient2 = vec3(0.2, 0.2, 0.5);
    }

    // Normals
    vec3 norm = normalize(normal);

    // First Light's Diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Second Light's Diffuse
    float diff2 = max(dot(norm, secondLightDir), 0.0);
    vec3 diffuse2 = diff2 * secondLightColor;

    // Specular Lighting for first light
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = spec * lightColor;

    // Combine the effects
    vec3 result = (ambient + diffuse + specular + ambient2 + diffuse2) * finalColor.rgb;
    result = mix(result, objectColor, 0.2); // Blend with object color

    FragColor = vec4(result, finalColor.a); // Use the alpha from the mixed texture
}
