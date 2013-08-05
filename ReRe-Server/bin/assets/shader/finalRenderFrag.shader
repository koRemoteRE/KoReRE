#version 330

in VertexData {
  vec3 viewDirVS;
  float pixelSizeVS;
  vec2 uv;
} In;

out vec4 outColor;

uniform sampler2D gBuffer_color;
uniform sampler2D gBuffer_pos;
uniform sampler2D gBuffer_normal;
uniform sampler2D gBuffer_tangent;
uniform sampler2D shadowMap;


uniform vec3 lightPos;
uniform vec3 lightDir;
uniform mat4 lightCamProjMat;
uniform mat4 lightCamviewMat;

uniform mat4 viewMat;
uniform mat4 iViewMat;

vec4 phong(in vec2 uv, in vec3 normal, in vec3 lVec, in vec3 vVec)
{
    vec4 vSpecular = vec4(0);
    vec4 vDiffuse = vec4(0);
    int shininess = 0;

    vec4 texColor = texture(gBuffer_color, uv);
    vVec = normalize(vVec);
    normal = normalize(normal);
    float intensity = 1;
    vec3 lVecNorm = normalize(lVec);
    float lambert = max(dot(normal,lVecNorm), 0.0);
    if(lambert > 0){
       vec4 lightcolor = vec4(1,1,1,1);
       vDiffuse = lightcolor * lambert * texColor * intensity ;
       vec3 R = normalize(reflect(-lVecNorm, normal));
       if(shininess>0){
            float specular = pow(clamp(dot(R, vVec), 0.0, 1.0), shininess);
            vSpecular = lightcolor * specular * intensity;
            }
    }
    return (vDiffuse + vSpecular);
}

void main(void)
{
  vec4 posVS = vec4(vec3(texture(gBuffer_pos, In.uv)),1);
  vec4 posWS = iViewMat * posVS;
  vec3 normalVS = normalize(texture(gBuffer_normal, In.uv).xyz);
  vec3 tangentVS = normalize(texture(gBuffer_tangent, In.uv).xyz);

  
  float visibility = 1.0;
  vec4 posLVS = lightCamviewMat * posWS;
  vec4 posLProj = lightCamProjMat * posLVS;
  posLProj.xyz /= posLProj.w;
  posLProj.xyz = posLProj.xyz * 0.5 + 0.5;
  
  float e = 0.001;
  if (abs(texture(shadowMap, posLProj.xy).x) + e < abs(posLProj.z)){
    visibility = 0.2;
  }

  outColor = visibility * phong(In.uv, normalVS, normalize(vec3(viewMat * vec4(lightDir,0))), vec3(posVS)) + vec4(0,0.2,0,1);
}