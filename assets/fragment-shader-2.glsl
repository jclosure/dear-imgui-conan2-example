#version 330 core

in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform vec3 lightColor;
uniform vec3 lightDir; // Direction of the light in world space

void main()
{
    // Normalize input vectors
    vec3 norm = normalize(Normal);
    vec3 lightDirection = normalize(-lightDir);

    // Diffuse shading
    float diff = max(dot(norm, lightDirection), 0.0);

    // Combine results
    vec3 diffuse = diff * lightColor;

    vec3 result = diffuse;
    FragColor = vec4(result, 1.0);
}
