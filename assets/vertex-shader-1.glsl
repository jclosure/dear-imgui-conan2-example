#version 330 core

layout(location = 0) in vec3 aPos;    // Vertex position
layout(location = 1) in vec3 aNormal; // Vertex normal

out vec3 FragPos;  // Position of the vertex in world space
out vec3 Normal;   // Normal vector in world space

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0)); // Calculate world-space position
    Normal = mat3(transpose(inverse(model))) * aNormal; // Transform normal to world space

    gl_Position = projection * view * vec4(FragPos, 1.0); // Final position
}
