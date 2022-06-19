#version 330 core
out vec4 FragColor;

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	sampler2D emission;

	float shininess;
};

struct Light {
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

in vec3 normal;
in vec3 fragPos;
in vec2 texCoord;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main()
{
	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(light.position - fragPos); 
	// vec3 lightDir = normalize(-light.direction); 

	float theta = dot(lightDir, normalize(-light.direction));
	
	float epsilon = light.innerCutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.f, 1.f);

	float distance = length(light.position  - fragPos);
	float attenuation = 1.f / (light.constant + light.linear * distance + light.quadratic * distance * distance);

	float diff = max(dot(norm, lightDir), 0.f);
	vec3 diffuse = light.diffuse * diff * texture(material.diffuse, texCoord).rgb;
	diffuse *= intensity;

	vec3 ambient = light.ambient * texture(material.diffuse, texCoord).rgb;

	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, norm); 
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * texture(material.specular, texCoord).rgb;
	specular *= intensity;

	vec3 emission = texture(material.emission, texCoord).rgb;
	emission = vec3(0.f);


	FragColor = vec4((emission + ambient + (diffuse + specular)*attenuation), 1.f);
} 