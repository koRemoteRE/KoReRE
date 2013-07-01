#version 150
 
uniform sampler2D frameTex;
uniform ivec2 texDim;
in vec4 FrameCoord;
in vec3 Color;
out vec4 outputF;
 
void main()
{
	vec2 texel;
	texel.x = 1.0/texDim.x;
	texel.y = 1.0/texDim.y;
	
	vec4 frameCoord = FrameCoord/FrameCoord.w;
	vec2 texCoord = (frameCoord.xy + vec2(1,1))/2;
	
	vec4 c = texture(frameTex,texCoord);
	if(c.w == 0){
		vec3 color = vec3(0.0);
		int num = 0;
		for(int x = -3; x <= 3; x++){
			for(int y = -3; y <= 3; y++){
				vec2 tempPos;
				tempPos.x = texCoord.x + x*texel.x;
				tempPos.y = texCoord.y + y*texel.y;
			
				vec4 tempColor = vec4(texture(frameTex,tempPos));
				if(tempColor.w != 0.0){
					num++;
					color += tempColor.xyz;
				}
			}
		}
		if(num != 0){
			outputF = vec4(color/num,1.0);
		}
	}
	else{
		outputF = vec4(c);
	}

}
