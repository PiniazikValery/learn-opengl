#version 330 core
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

out vec4 FragColor;

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

struct PointLight {    
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;  
 
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct DirectionLight {
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
uniform DirectionLight dirLight;
#define MAX_POINT_LIGHTS 50
uniform int pointLightsCount;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
#define MAX_SPOT_LIGHTS 50
uniform int spotLightsCount;
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];

vec3 CalcDirLight(DirectionLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight pointLight, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight spotLight, vec3 normal, vec3 fragPos, vec3 viewDir);

vec3 CalcDirLight(DirectionLight light, vec3 normal, vec3 viewDir) 
{
    // Ambient
    vec3 ambient = vec3(texture(material.diffuse, TexCoords)) * light.ambient;

    // Diffuse
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = (diff * vec3(texture(material.diffuse, TexCoords))) * light.diffuse;

    // Specular
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = (vec3(texture(material.specular, TexCoords)) * spec) * light.specular;

    return ambient + diffuse + specular;
}

vec3 CalcPointLight(PointLight pointLight, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    // Ambient
    vec3 ambient = vec3(texture(material.diffuse, TexCoords)) * pointLight.ambient;

    // Diffuse
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(pointLight.position - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = (diff * vec3(texture(material.diffuse, TexCoords))) * pointLight.diffuse;

    // Specular
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = (vec3(texture(material.specular, TexCoords)) * spec) * pointLight.specular;

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
    vec3 ambient = vec3(texture(material.diffuse, TexCoords)) * spotLight.ambient;

    // Diffuse
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(spotLight.position - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = (diff * vec3(texture(material.diffuse, TexCoords))) * spotLight.diffuse;

    // Specular
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = (vec3(texture(material.specular, TexCoords)) * spec) * spotLight.specular;

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
   vec3 viewDir = normalize(viewPos-FragPos);
   
   vec3 color = CalcDirLight(dirLight, Normal, viewDir);

   for(int i = 0; i < pointLightsCount; i++)
   {
       color += CalcPointLight(pointLights[i], Normal, FragPos, viewDir);
   }

   for(int i = 0; i < spotLightsCount; i++)
   {
       color += CalcSpotLight(spotLights[i], Normal, FragPos, viewDir);
   }

    // Emission
    vec3 emission = vec3(0.0);
    if(material.withEmission)
    {
        vec3 cut = vec3(0.0);
        emission = vec3(texture(material.emission, TexCoords));
        emission *= step(vec3(texture(material.specular, TexCoords)), cut);
    }

    FragColor = vec4(color + emission, 1.0);
}