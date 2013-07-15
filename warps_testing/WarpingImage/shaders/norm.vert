uniform mat4 viewMatrix, projMatrix;

// View- and projection matrices of the keyframe
uniform mat4 oldViewMatrix,oldProjMatrix;

in vec4 position;
in vec2 uvAttr;

out vec4 FrameCoord;
out vec2 Uv;
void main()
{

    gl_Position = projMatrix * viewMatrix * position ;
 	FrameCoord = oldProjMatrix * oldViewMatrix * position ;
 	Uv =uvAttr;
}