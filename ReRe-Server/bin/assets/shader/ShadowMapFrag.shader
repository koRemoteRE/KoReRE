#version 330

out vec4 color;

in vec4 posWS;
 
void main(){
  color = posWS;
}