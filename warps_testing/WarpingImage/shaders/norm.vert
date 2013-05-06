uniform mat4 viewMatrix, projMatrix;

// View- and projection matrices of the keyframe
uniform mat4 oldViewMatrix,oldProjMatrix;

in vec4 position;

out vec4 FrameCoord;
void main()
{

    gl_Position = projMatrix * viewMatrix * position ;
 	FrameCoord = oldProjMatrix * oldViewMatrix * position ;
}