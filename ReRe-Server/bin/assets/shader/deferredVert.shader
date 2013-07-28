#version 330
// vertex position in modelspace
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec3 v_tangent;
layout(location = 3) in vec3 v_uv0;

uniform mat4 projectionMat;
uniform mat4 viewMat;
uniform mat4 modelMat;
uniform mat3 normalMat;

// World-space vectors
out VertexData {
  vec3 positionVS;
  vec3 normalVS;
  vec3 tangentVS;
  vec3 bitangentVS;
  vec2 uv;
} Out;

void main()
{
  Out.positionVS = (viewMat * modelMat * vec4(v_position, 1.0)).xyz;
  gl_Position = projectionMat * vec4(Out.positionVS, 1.0);

  Out.normalVS = normalize((viewMat * vec4(normalMat * v_normal,0)).xyz);
  Out.tangentVS = abs(normalize((viewMat * modelMat * vec4(v_tangent,0)).xyz));
  Out.bitangentVS = normalize(cross(Out.normalVS, Out.tangentVS));

  Out.uv = v_uv0.xy;
}
