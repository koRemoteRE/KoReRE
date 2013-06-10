#version 150
 
// Incoming color
in vec3 Color;
in vec2 Uv;

uniform sampler2D frameTex;

out vec4 outputF;

void main()
{
	
	outputF = texture2D(frameTex, Uv.xy);//vec4(Color,1.0);

}
