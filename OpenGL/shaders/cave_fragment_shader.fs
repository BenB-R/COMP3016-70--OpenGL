#version 330 core
out vec4 FragColor;

in vec3 normal; // Received from the vertex shader

uniform vec3 objectColor; // Color of the object
uniform vec3 lightColor;  // Color of the light
uniform vec3 lightDir;    // Direction of the light
uniform vec3 ambientStrength; // Strength of the ambient lighting


void main()
{
    // Ambient
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse
    float diff = max(dot(normalize(normal), -lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Combine results
    vec3 result = (ambient + diffuse) * objectColor;
    FragColor = vec4(result, 1.0);
}
