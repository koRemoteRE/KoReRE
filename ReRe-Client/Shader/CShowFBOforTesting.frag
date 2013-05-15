#version 150

in vec4 f_position;
in vec3 f_texture;

uniform sampler2D diffImgTexture;

uniform int textureToShow;

out vec4 out_color;

void main()
{
    out_color = texture(diffImgTexture, f_texture.st);
}