#version 150

uniform mat4 m_view, m_projection,m_model;

// View- and projection matrices of the keyframe
uniform mat4 oldViewMatrix,oldProjMatrix;

in vec3 v_position;

out vec4 FrameCoord;
out vec4 pos_proj;
void main()
{

    gl_Position = m_projection * m_view * m_model*vec4(v_position,1.0) ;
    pos_proj = gl_Position;
    FrameCoord = oldProjMatrix * oldViewMatrix *m_model* vec4(v_position,1.0) ;
}
