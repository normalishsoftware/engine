#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 tex_coord;

out vec2 _tex_coord;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.f);
	_tex_coord = vec2(tex_coord.x, 1.f - tex_coord.y);
}

#shader fragment
#version 330 core

in vec2 _tex_coord;
out vec4 _color;
uniform sampler2D _texture;

void main()
{
	_color = texture(_texture, _tex_coord);
}