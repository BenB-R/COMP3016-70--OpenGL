#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform sampler2D texture1;
uniform vec3 viewPos; // Camera position

// uniforms to control the glow effect
uniform float maxGlowIntensity; // The maximum intensity of the glow
uniform float glowVisibilityDistance; // The distance at which the glow is fully visible
uniform float glowFactor; // A factor to adjust the attenuation of glow over distance

void main() {
    // Texture color
    vec3 textureColor = texture(texture1, TexCoords).rgb;
    
    // Calculate the distance from the camera to the fragment
    float distance = length(viewPos - FragPos);

    // Adjust the glow intensity based on the distance, with a cap on the maximum intensity
    float attenuation = 1.0 / (1.0 + glowFactor * distance * distance);
    float glowIntensity = clamp(attenuation, 0.0, maxGlowIntensity); // Use the uniform to clamp the value

    // Apply glow based on glowColor 
    vec3 glowColor = vec3(0.0, 1.0, 0.0); // Example green glow for visibility
    vec3 colorWithGlow = mix(textureColor, glowColor, glowIntensity);

    // Final color with glow applied
    FragColor = vec4(colorWithGlow, 1.0);
}
