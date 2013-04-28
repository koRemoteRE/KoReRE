
#version 150
 
uniform sampler2D frameTex;
in vec4 FrameCoord;
in vec3 Color;
out vec4 outputF;
 
void main()
{
	vec4 frameCoord = FrameCoord/FrameCoord.w;
	vec2 texCoord = (frameCoord.xy + vec2(1,1))/2;
	outputF = vec4(texture2D(frameTex,texCoord).xyz,1.0);

}
