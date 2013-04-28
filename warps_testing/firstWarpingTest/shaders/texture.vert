
in vec2 position;

 out vec2 texturePosition;

void main()
{

    texturePosition = (position+1.0)/2.0;
    gl_Position = vec4(position,0.0,1.0) ;
 
}