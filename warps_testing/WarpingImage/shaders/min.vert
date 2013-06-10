uniform mat4 viewMatrix, projMatrix;

// Incoming position and color per vertex
in vec4 position;
in vec3 color;
in vec2 uvAttr;
 
out vec3 Color;
out vec2 Uv;
 
void main()
{
    Color = color;
    gl_Position = projMatrix * viewMatrix * position ;
    Uv = uvAttr;
    
}