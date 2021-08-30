#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 Pos;

struct DirectionLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {    
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;  
 
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;

    vec3 direction;

    float cutOff;
    float outerCutOff;
    
    float constant;
    float linear;
    float quadratic;  
 
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Material {
    sampler2D diffuse1;
    sampler2D specular1;
    sampler2D emission1;
    bool withEmission;
    float shininess;
};

vec3 CalcDirLight(DirectionLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight pointLight, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight spotLight, vec3 normal, vec3 fragPos, vec3 viewDir);

uniform Material material;
uniform vec3 viewPos;
uniform DirectionLight dirLight;
#define MAX_POINT_LIGHTS 50
uniform int pointLightsCount;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
#define MAX_SPOT_LIGHTS 50
uniform int spotLightsCount;
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];

vec3 CalcDirLight(DirectionLight light, vec3 normal, vec3 viewDir) 
{
    // Ambient
    vec3 ambient = vec3(texture(material.diffuse1, TexCoords)) * light.ambient;

    // Diffuse
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = (diff * vec3(texture(material.diffuse1, TexCoords))) * light.diffuse;

    // Specular
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = (vec3(texture(material.specular1, TexCoords).rrr) * spec) * light.specular;

    return ambient + diffuse + specular;
}

vec3 CalcPointLight(PointLight pointLight, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    // Ambient
    vec3 ambient = vec3(texture(material.diffuse1, TexCoords)) * pointLight.ambient;

    // Diffuse
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(pointLight.position - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = (diff * vec3(texture(material.diffuse1, TexCoords))) * pointLight.diffuse;

    // Specular
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = (vec3(texture(material.specular1, TexCoords).rrr) * spec) * pointLight.specular;

    // Attenuation
    float distance = length(pointLight.position - fragPos);
    float attenuation = 1.0 / (pointLight.constant + pointLight.linear * distance + pointLight.quadratic * (distance * distance));

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return ambient + diffuse + specular;
}

vec3 CalcSpotLight(SpotLight spotLight, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    // Ambient
    vec3 ambient = vec3(texture(material.diffuse1, TexCoords)) * spotLight.ambient;

    // Diffuse
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(spotLight.position - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = (diff * vec3(texture(material.diffuse1, TexCoords))) * spotLight.diffuse;

    // Specular
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = (vec3(texture(material.specular1, TexCoords).rrr) * spec) * spotLight.specular;

    // Attenuation
    float distance = length(spotLight.position - fragPos);
    float attenuation = 1.0 / (spotLight.constant + spotLight.linear * distance + spotLight.quadratic * (distance * distance));

    // Spotlight
    float theta = dot(lightDir, normalize(-spotLight.direction));
    float epsilon   = spotLight.cutOff - spotLight.outerCutOff;
    float intensity = clamp((theta - spotLight.outerCutOff) / epsilon, 0.0, 1.0);   

    ambient *= attenuation;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return ambient + diffuse + specular;
}

void main()
{    
    vec3 viewDir = normalize(viewPos-Pos);

    vec3 color = CalcDirLight(dirLight, Normal, viewDir);

    for(int i = 0; i < pointLightsCount; i++)
    {
        color += CalcPointLight(pointLights[i], Normal, Pos, viewDir);
    }

    for(int i = 0; i < spotLightsCount; i++)
    {
       color += CalcSpotLight(spotLights[i], Normal, Pos, viewDir);
    }

    FragColor = vec4(color, 1.0);
}