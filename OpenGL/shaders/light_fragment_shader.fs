#version 330 core
out vec4 FragColor;

in vec3 Normal; // Normal vector passed from the vertex shader
in vec3 FragPos; // Fragment position passed from the vertex shader

struct Light {
    vec3 position;
    vec3 color;
};

uniform Light light;
uniform vec3 viewPos; // Camera position

void main()
{
    // Ambient component
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * light.color;

    // Diffuse component
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light.color;

    // Specular component
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * light.color;

    vec3 result = (ambient + diffuse + specular) * texture(texture_diffuse1, TexCoords).rgb;
    FragColor = vec4(result, 1.0);
}
