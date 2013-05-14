#version 150

in vec3 v_position;
in vec2 v_texture;

uniform mat4 m_projection;
uniform mat4 m_view;
uniform mat4 m_model;

out vec4 f_position;
out vec3 f_texture;

void main()
{
	f_position = m_projection * m_view * m_model * vec4(v_position, 1.0f);
	f_texture = vec3(v_texture, 1.0f);
}
