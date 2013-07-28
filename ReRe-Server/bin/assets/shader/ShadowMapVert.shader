#version 330 core

layout (location = 0) in vec3 v_position;
out vec4 posWS;
 
uniform mat4 viewProjMat;
uniform mat4 modelMat;
 
void main(){
  posWS = modelMat* vec4(v_position,1);
  gl_Position = viewProjMat * posWS;
}