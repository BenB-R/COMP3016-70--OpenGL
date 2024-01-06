#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord; // Added texture coordinates

out vec3 normal;
out vec3 FragPos;   // Output for world position
out vec2 TexCoord; // Pass texture coordinates to fragment shader

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    normal = mat3(transpose(inverse(model))) * aNormal;  // Transform normals
    FragPos = vec3(model * vec4(aPos, 1.0));  // Calculate world position
    TexCoord = aTexCoord; // Pass texture coordinates to the fragment shader
}
