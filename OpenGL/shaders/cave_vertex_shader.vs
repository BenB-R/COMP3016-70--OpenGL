#version 330 core
layout (location = 0) in vec3 aPos;   // The position variable has attribute position 0
layout (location = 1) in vec3 aNormal; // The normal variable has attribute position 1

out vec3 normal; // For passing to the fragment shader

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    
    // Transform the normals to world space and pass to the fragment shader
    normal = mat3(transpose(inverse(model))) * aNormal;
}
