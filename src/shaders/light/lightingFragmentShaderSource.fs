#version 330 core
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

out vec4 FragColor;

struct Light {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    bool withEmission;
    float shininess;
};

uniform vec3 viewPos;
uniform Material material;
uniform Light light;
 
void main()
{
    // Ambient
    vec3 ambient = vec3(texture(material.diffuse, TexCoords)) * light.ambient;

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = (diff * vec3(texture(material.diffuse, TexCoords))) * light.diffuse;

    // Specular
    vec3 viewDir = normalize(viewPos-FragPos);
    vec3 reflectDir = reflect(-lightDir, norm); 
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = (vec3(texture(material.specular, TexCoords)) * spec) * light.specular; 

    // Emission
    vec3 emission = vec3(0.0);
    if(material.withEmission)
    {
        vec3 cut = vec3(0.0);
        emission = vec3(texture(material.emission, TexCoords));
        emission *= step(vec3(texture(material.specular, TexCoords)), cut);
    }

    vec3 result = ambient + diffuse + specular + emission;
    FragColor = vec4(result, 1.0);
}