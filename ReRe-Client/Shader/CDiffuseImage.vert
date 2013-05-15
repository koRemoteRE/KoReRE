#version 150

in vec3 v_position;
in vec3 v_normal;
//in vec2 v_texture;

uniform vec3 v_lightPos;

uniform mat4 m_projection;
uniform mat4 m_view;
uniform mat4 m_model;
uniform mat4 m_normal;

out vec3 f_lightVec;
out vec4 f_position;
out vec3 f_normal;
//out vec3 f_texture;

void main()
{
    f_normal = normalize((m_normal * vec4(v_normal, 0.0f)).xyz);
//  f_texture = vec3(v_texture, 1.0f);
    
    vec4 transformed_position = m_view * m_model * vec4(v_position, 1.0f);
    transformed_position.xyz = transformed_position.xyz / transformed_position.w;
    f_lightVec = normalize(v_lightPos - transformed_position.xyz);
    
    f_position = m_projection * m_view * m_model * vec4(v_position, 1.0f);
}