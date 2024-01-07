#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 viewPos; // Camera position
uniform vec3 glowColor = vec3(1.0, 0.0, 0.0); // Dim blue color

void main()
{
    // Calculate the distance from the camera to the fragment
    float distance = length(viewPos - FragPos);

    // Calculate the glow intensity based on the distance
    float glowIntensity = 1.0 / (distance * distance);

    glowIntensity = clamp(glowIntensity, 0.0, 1.0); // Ensure it's between 0 and 1

    // Base color of the crystal (adjust as needed)
    vec3 baseColor = vec3(0.2, 0.3, 0.7); 

    // Blend the base color with the glow color
    vec3 color = mix(baseColor, glowColor, glowIntensity);

    FragColor = vec4(color, 1.0);
}
