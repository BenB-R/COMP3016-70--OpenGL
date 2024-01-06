#version 330 core
out vec4 FragColor;

in vec3 normal; // Received from the vertex shader
in vec2 TexCoord; // Texture coordinates passed from the vertex shader
in vec3 FragPos; // World position passed from the vertex shader

uniform sampler2D texture1;

uniform vec3 objectColor; // Color of the object
uniform vec3 lightColor;  // Color of the first light
uniform vec3 lightDir;    // Direction of the first light
uniform vec3 ambientStrength; // Strength of the ambient lighting

uniform vec3 secondLightDir;    // Second light direction
uniform vec3 secondLightColor;  // Second light color
uniform vec3 secondAmbientStrength; // Second light ambient strength

uniform vec3 viewPos; // Camera position for specular calculation

void main()
{
    // Sample the texture color
    vec4 texColor = texture(texture1, TexCoord);

    // Ambient
    vec3 ambient = ambientStrength * lightColor * 1.5; // Increased ambient strength

    // Normals
    vec3 norm = normalize(normal);

    // First Light's Diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Second Light's Diffuse
    float diff2 = max(dot(norm, secondLightDir), 0.0);
    vec3 diffuse2 = diff2 * secondLightColor;

    // Specular Lighting
    vec3 viewDir = normalize(viewPos - FragPos); // Corrected view direction
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = spec * lightColor * 0.5; // Specular component for the first light

    // Combine the effects
    vec3 result = (ambient + diffuse + diffuse2 + specular) * texColor.rgb;
    result = mix(result, objectColor, 0.2); // Blend with object color

    FragColor = vec4(result, texColor.a); // Use the alpha from the texture
}
