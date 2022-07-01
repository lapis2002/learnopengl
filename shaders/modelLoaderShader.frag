#version 330 core
out vec4 fragColor;

in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;


#define NR_POINT_LIGHTS 4  

struct DirLight {
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
	vec3 diffuse;
	vec3 ambient;
	vec3 specular;

	vec3 position;
	vec3 direction;

	// attenuation
	float constant;
	float linear;
	float quadratic;

	// spotlight
	float innerCutOff;
	float outerCutOff;
};

struct Material {
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
    sampler2D texture_normal1;

	sampler2D emission;

	float shininess;
};

vec3 calDirLight(DirLight light, vec3 normal, vec3 viewDir);  
vec3 calPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 calSpotLight(SpotLight light, vec3 normal, vec3 norm, vec3 fragPos, vec3 viewDir);

uniform vec3 viewPos;
uniform Material material;
uniform SpotLight light;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform mat4 modelMat;


void main() {    
    vec3 norm = normalize(mat3(transpose(inverse(modelMat))) * texture(material.texture_normal1, texCoord).xyz);
	vec3 lightDir = normalize(light.position - fragPos); 
    vec3 viewDir = normalize(viewPos - fragPos);

    // phase 1: Directional lighting
    vec3 result = vec3(0.f);
    
    //result += calDirLight(dirLight, norm, viewDir);
    // phase 2: Point lights
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += calPointLight(pointLights[i], norm, fragPos, viewDir);    
    // phase 3: Spot light
    result += calSpotLight(light, normal, norm, fragPos, viewDir);  

    //fragColor = texture(material.texture_diffuse1, texCoord);
    fragColor = vec4(result, 1.f);
}

vec3 calDirLight(DirLight light, vec3 normal, vec3 viewDir) {
	vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(material.texture_diffuse1, texCoord));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.texture_diffuse1, texCoord));
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, texCoord));
    return (ambient + diffuse + specular);
}

vec3 calPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(material.texture_diffuse1, texCoord));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.texture_diffuse1, texCoord));
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, texCoord));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
} 
vec3 calSpotLight(SpotLight light, vec3 normal, vec3 norm, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    float theta = dot(lightDir, normalize(-light.direction));
	
	float epsilon = light.innerCutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.f, 1.f);

	float distance = length(light.position  - fragPos);
	float attenuation = 1.f / (light.constant + light.linear * distance + light.quadratic * distance * distance);

	float diff = max(dot(norm, lightDir), 0.f);
	vec3 diffuse = light.diffuse * diff * texture(material.texture_diffuse1, texCoord).rgb;
	diffuse *= intensity;

	vec3 ambient = light.ambient * texture(material.texture_diffuse1, texCoord).rgb;

	vec3 reflectDir = reflect(-lightDir, norm); 
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * texture(material.texture_specular1, texCoord).rgb;
	specular *= intensity;

	vec3 emission = texture(material.emission, texCoord).rgb;

    return  (ambient + diffuse + specular);
}