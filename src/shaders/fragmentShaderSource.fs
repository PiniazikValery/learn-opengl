#version 330 core
out vec4 FragColor;
  
in vec3 ourColor;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
in vec3 LightPos;
 
uniform sampler2D box_texture;
uniform sampler2D smile_texture;
uniform float textureLevel;
uniform vec3 lightColor;
uniform vec3 viewPos;
 
void main()
{
    // Ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(-FragPos);
    vec3 reflectDir = reflect(-lightDir, norm); 
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    FragColor = mix(texture(box_texture, TexCoord)* vec4((ambient + diffuse + specular), 1.0), texture(smile_texture, vec2(1.0 - TexCoord.x, TexCoord.y)), textureLevel);
}