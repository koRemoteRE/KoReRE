#version 330

in VertexData {
  vec3 positionVS;
  vec3 normalVS;
  vec3 tangentVS;
  vec3 bitangentVS;
  vec2 uv;
} In;

uniform sampler2D	diffuseTex;
uniform sampler2D normalTex;
uniform uint hasNM;

out vec4 color[4];

void main(void)
{
  color[0] = vec4(texture(diffuseTex, vec2(In.uv.x, 1.0 - In.uv.y)).rgb, 0);
  color[1] = vec4(In.positionVS, 0);
  
  if(hasNM == 0U){ 
    color[2] = vec4(In.normalVS, 0);
  }else{
    vec4 normalMap = vec4(texture(normalTex, vec2(In.uv.x, 1.0 - In.uv.y)).rgb, 0);
    color[2] = vec4(normalize(In.normalVS + In.tangentVS * normalMap.x + In.bitangentVS * normalMap.y),0); 
  }
  color[3] = vec4(In.tangentVS, 0);
}
