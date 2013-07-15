
#version 150
 
uniform sampler2D frameTex;
uniform sampler2D objectTex;
uniform ivec2 texDim;
in vec2 texturePosition;
in vec4 FrameCoord;
in vec3 Color;
in vec2 Uv;
out vec4 outputF;
 
void main()
{
	vec2 texel;
	texel.x = 1.0/texDim.x;
	texel.y = 1.0/texDim.y;
	
	vec4 frameCoord = FrameCoord/FrameCoord.w;
	vec2 texCoord = (frameCoord.xy + vec2(1,1))/2;
	
	vec4 c = texture2D(frameTex,texCoord);
	if(c.w == 0){
		outputF = texture2D(objectTex,Uv.xy);
		
		//outputF = vec4(0.0,1.0,0.0,1.0);
		//vec3 color = vec3(0.0);
		//int num = 0;
		
		//for(int x = -1; x <= 1; x++){
		//	for(int y = -1; y <= 1; y++){
		//		vec2 tempPos;
		//		tempPos.x = texCoord.x + x*texel.x;
		//		tempPos.y = texCoord.y + y*texel.y;
			
		//		vec4 tempColor = vec4(texture2D(frameTex,tempPos));
		//		if(tempColor.w != 0.0){
		//			num++;
		//			color += tempColor.xyz;
		//		}
		//	}
		//}
		
		//if(num != 0){
		//	outputF = vec4(color/num,1.0);
		//}
	}
	else{
		outputF = vec4(c);
	}
}
