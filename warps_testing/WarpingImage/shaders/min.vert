uniform mat4 modelMatrix, viewMatrix, projMatrix;

// Incoming position and color per vertex
in vec4 position;
in vec3 color;
 
out vec3 Color;
 
void main()
{
    Color = color;
    gl_Position = projMatrix * viewMatrix * modelMatrix * position ;
    
}