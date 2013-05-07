#version 320

in vec4 f_position;
in vec3 f_normal;
in vec3 f_texture;
in vec3 f_lightVec;

uniform sampler2D texture;
uniform vec3 diffuseFrontMaterial;
uniform vec3 diffuseLightColor;

layout (location = 0) out vec4 out_diffColor;
layout (location = 1) out vec4 out_depthColor;

void main()
{
    float cosinus = max(0.0f, dot(f_normal, f_lightVec));
    vec4 texel = texture2D(texture, f_texture.st);
    
    vec4 diffuseReflection = vec4( diffuseFrontMaterial * diffuseLightColor * cosinus, 1.0f);
    
    out_diffColor = texel * diffuseReflection;
    
    // TODO: Calculate Depth Color???
}