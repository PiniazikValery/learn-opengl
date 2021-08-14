#version 330 core
out vec4 FragColor;
  
in vec3 ourColor;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};
 
uniform sampler2D box_texture;
uniform sampler2D smile_texture;
uniform float textureLevel;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform Material material;
uniform Light light;
 
void main()
{
    // Ambient
    vec3 ambient = material.ambient * light.ambient;

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = (diff * material.diffuse) * light.diffuse;

    // Specular
    vec3 viewDir = normalize(-FragPos);
    vec3 reflectDir = reflect(-lightDir, norm); 
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = (material.specular * spec) * light.specular;

    FragColor = mix(texture(box_texture, TexCoord)* vec4((ambient + diffuse + specular), 1.0), texture(smile_texture, vec2(1.0 - TexCoord.x, TexCoord.y)), textureLevel);
}