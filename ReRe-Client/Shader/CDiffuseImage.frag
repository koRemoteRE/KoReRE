#version 330

in vec4 f_position;
in vec3 f_normal;
in vec3 f_texture;
in vec3 f_lightVec;

uniform sampler2D texture;
uniform vec3 diffuseFrontMaterial;
uniform vec3 diffuseLightColor;

out vec4 out_color;

void main()
{
    float cosinus = max(0.0f, dot(f_normal, f_lightVec));
    vec4 texel = texture2D(texture, f_texture.st);
    
    vec4 diffuseReflection = vec4( diffuseFrontMaterial * diffuseLightColor * cosinus, 1.0f);
    
    out_color = texel * diffuseReflection;
}