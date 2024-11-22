#version 330 core

in vec3 FragPos;  // Interpolated position of the fragment in world space
in vec3 Normal;   // Interpolated normal vector in world space

out vec4 FragColor; // Final fragment color

uniform vec3 lightDir;   // Directional light direction
uniform vec3 lightColor; // Color of the light

void main() {
    // Normalize the normal and light direction vectors
    vec3 norm = normalize(Normal);
    vec3 lightDirNorm = normalize(-lightDir);

    // Calculate diffuse lighting using the Lambertian reflection model
    float diff = max(dot(norm, lightDirNorm), 0.0);

    // Combine diffuse lighting with the light color
    vec3 diffuse = diff * lightColor;

    // Set the object color
    vec3 objectColor = vec3(0.8, 0.5, 0.3); // Example: brownish cuboid

    // Final color
    vec3 result = (diffuse + vec3(0.2)) * objectColor; // Add ambient light
    FragColor = vec4(result, 1.0);
}
