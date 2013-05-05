
#version 150
 
uniform sampler2D frameTex;
in vec2 texturePosition;
out vec4 outputF;
 
void main()
{
	outputF = vec4(texture2D(frameTex,texturePosition).xyz,1.0);
}
