#version 150

in vec3 v_position;
in vec3 v_normal;
in vec2 v_texture;

uniform mat4 m_projection;
uniform mat4 m_view;
uniform mat4 m_model;
uniform mat3 m_normal;

uniform vec3 lightPos;

out vec3 f_lightVec;
out vec3 f_normal;
out vec3 f_texture;

void main()
{
    f_normal = normalize(m_normal * v_normal);
    f_texture = vec3(v_texture, 1.0f);
    
    vec4 transformed_position = m_view * m_model * vec4(v_position, 1.0f);
    transformed_position.xyz = transformed_position.xyz / transformed_position.w;
    vec4 lightVec = m_view * vec4(lightPos, 1.0f);
    f_lightVec = normalize(lightVec.xyz - transformed_position.xyz);

    gl_Position = m_projection * m_view * m_model * vec4(v_position, 1.0f);
}