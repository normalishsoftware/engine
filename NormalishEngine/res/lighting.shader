#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 tex_coord;

out vec3 _normal;
out vec3 _fragment_position;
out vec2 _tex_coord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.f);
	_fragment_position = vec3(model * vec4(position, 1.f));
	_normal = mat3(transpose(inverse(model))) * normal;
	_tex_coord = tex_coord;
}

#shader fragment
#version 330 core

#define NUMBER_OF_POINT_LIGHTS 4

struct Material
{
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

struct DirectionalLight
{
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight
{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float constant;
	float linear;
	float quadratic;
};

struct SpotLight
{
	vec3 position;
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float constant;
	float linear;
	float quadratic;
	float cutoff;
	float outer_cutoff;
};

out vec4 _color;

in vec2 _tex_coord;
in vec3 _normal;
in vec3 _fragment_position;

uniform vec3 view_position;
uniform Material material;
uniform DirectionalLight directional_light;
uniform PointLight point_lights[NUMBER_OF_POINT_LIGHTS];
uniform SpotLight spot_light;

vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 view_direction)
{
	vec3 light_direction = normalize(-light.direction);
	float diff = max(dot(normal, light_direction), 0.f);
	vec3 reflection_direction = reflect(-light_direction, normal);
	float spec = pow(max(dot(view_direction, reflection_direction), 0.f), material.shininess);

	vec3 ambient = light.ambient * vec3(texture(material.diffuse, _tex_coord));
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, _tex_coord));
	vec3 specular = light.specular * spec * vec3(texture(material.specular, _tex_coord));

	return ambient + diffuse + specular;
}

vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 frag_position, vec3 view_direction)
{
	vec3 light_direction = normalize(light.position - frag_position);
	float diff = max(dot(normal, light_direction), 0.f);
	vec3 reflection_direction = reflect(-light_direction, normal);
	float spec = pow(max(dot(view_direction, reflection_direction), 0.f), material.shininess);

	float distance = length(light.position - frag_position);
	float attenuation = 1.f / (light.constant + light.linear * distance + light.quadratic * distance * distance);

	vec3 ambient = light.ambient * vec3(texture(material.diffuse, _tex_coord));
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, _tex_coord));
	vec3 specular = light.specular * spec * vec3(texture(material.specular, _tex_coord));

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return ambient + diffuse + specular;
}

vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 frag_position, vec3 view_direction)
{
	vec3 light_direction = normalize(light.position - frag_position);
	float diff = max(dot(normal, light_direction), 0.f);
	vec3 reflection_direction = reflect(-light_direction, normal);
	float spec = pow(max(dot(view_direction, reflection_direction), 0.f), material.shininess);

	float distance = length(light.position - frag_position);
	float attenuation = 1.f / (light.constant + light.linear * distance + light.quadratic * distance * distance);

	float theta = dot(light_direction, normalize(-light.direction));
	float epsilon = light.cutoff - light.outer_cutoff;
	float intensity = clamp((theta - light.outer_cutoff) / epsilon, 0.f, 1.f);

	vec3 ambient = light.ambient * vec3(texture(material.diffuse, _tex_coord));
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, _tex_coord));
	vec3 specular = light.specular * spec * vec3(texture(material.specular, _tex_coord));

	ambient *= attenuation * intensity;
	diffuse *= attenuation * intensity;
	specular *= attenuation * intensity;

	return ambient + diffuse + specular;
}

void main()
{
	vec3 norm = normalize(_normal);
	vec3 view_direction = normalize(view_position - _fragment_position);

	vec3 result = CalculateDirectionalLight(directional_light, norm, view_direction);

	for (int i = 0; i < NUMBER_OF_POINT_LIGHTS; i++)
	{
		result += CalculatePointLight(point_lights[i], norm, _fragment_position, view_direction);
	}

	result += CalculateSpotLight(spot_light, norm, _fragment_position, view_direction);

	_color = vec4(result, 1.f);
}