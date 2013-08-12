#version 150
 
uniform sampler2D frameTex;
uniform sampler2D diffTex;
uniform ivec2 texDim;
in vec4 FrameCoord;
in vec3 Color;
in vec4 pos_proj;

out vec4 outputF;
 
void main()
{
	vec2 texel;
	texel.x = 1.0/texDim.x;
	texel.y = 1.0/texDim.y;
	
	vec4 frameCoord = FrameCoord/FrameCoord.w;
	vec2 texCoord = (frameCoord.xy + vec2(1,1))/2;
	
	vec4 c = texture(frameTex,texCoord);
	if(texCoord.x < 0 || texCoord.x > 1 || texCoord.y < 0 || texCoord.y > 1 || c.w == 0.0){
		outputF = vec4(1.0,0.0,0.0,1.0);

		vec4 pos_screen = (pos_proj/pos_proj.w+1.0)/2.0;
		outputF =  texture(diffTex,pos_screen.xy) + vec4(0.3,0.0,0.0,0.0);

	}
	else{
		outputF = vec4(c);
	}		
}
