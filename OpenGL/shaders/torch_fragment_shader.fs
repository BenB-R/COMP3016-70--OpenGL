#version 330 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;  
in vec2 TexCoords;

uniform sampler2D texture1;
uniform vec3 lightPos; 
uniform vec3 viewPos; 
uniform vec3 lightColor;

void main()
{
    // Texture color
    vec4 texColor = texture(texture1, TexCoords);

    // Simulate orange glow at the top of the torch
    float intensity = pow(max(dot(normalize(Normal), vec3(0, 1, 0)), 0.0), 2.0);
    vec3 glowColor = intensity * vec3(1.0, 0.5, 0.0); // Orange glow

    // Combine texture color with the glow
    vec3 finalColor = mix(texColor.rgb, glowColor, intensity);

    FragColor = vec4(finalColor, texColor.a);
}
