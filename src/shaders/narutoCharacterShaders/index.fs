#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

struct Material {
    sampler2D diffuse1;
};

uniform Material material;

void main()
{    
    FragColor = vec4(vec3(texture(material.diffuse1, TexCoords)), 1.0);
}