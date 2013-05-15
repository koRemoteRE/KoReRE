#version 150

in vec4 f_position;
in vec3 f_texture;

uniform sampler2D diffImgTexture;
uniform sampler2D depthImgTexture;

uniform int textureToShow;

out vec4 out_color;

void main()
{
	if(textureToShow == 0) // show diffImg
    out_color = texture(diffImgTexture, f_texture.st);
	else // show depthImg
    out_color = texture(depthImgTexture, f_texture.st);
}