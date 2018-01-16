#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec2 tex_coord;

out vec3 _color;
out vec2 _tex_coord;
uniform mat4 _transform;

void main()
{
	gl_Position = _transform * vec4(position, 1.f);
	_color = color;
	_tex_coord = vec2(tex_coord.x, 1.f - tex_coord.y);
}

#shader fragment
#version 330 core

in vec3 _color;
in vec2 _tex_coord;
out vec4 color;
uniform sampler2D _texture;

void main()
{
	color = texture(_texture, _tex_coord);
}