#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

out vec3 _normal;
out vec3 _fragment_position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.f);
	_fragment_position = vec3(model * vec4(position, 1.f));
	_normal = mat3(transpose(inverse(model))) * normal;
}

#shader fragment
#version 330 core

struct Material
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct Light
{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

out vec4 _color;

in vec3 _normal;
in vec3 _fragment_position;

uniform vec3 view_position;
uniform Material material;
uniform Light light;

void main()
{
	vec3 ambient = light.ambient * material.ambient;

	vec3 norm = normalize(_normal);
	vec3 light_direction = normalize(light.position - _fragment_position);
	float diff = max(dot(norm, light_direction), 0.f);
	vec3 diffuse = diff * material.diffuse * light.diffuse;

	vec3 view_direction = normalize(view_position - _fragment_position);
	vec3 reflect_direction = reflect(-light_direction, norm);
	float spec = pow(max(dot(view_direction, reflect_direction), 0.f), material.shininess);
	vec3 specular = spec * material.specular * light.specular;

	vec3 result = ambient + diffuse + specular;
	_color = vec4(result, 1.f);
}