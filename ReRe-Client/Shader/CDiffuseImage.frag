#version 150

in vec3 f_lightVec;
in vec4 f_position;
in vec3 f_normal;
//in vec3 f_texture;

//uniform sampler2D texture;
uniform vec3 diffuseFrontMaterial;
uniform vec3 diffuseLightColor;

out vec4 out_diffColor;

void main()
{
    float cosinus = max(0.0f, dot(f_normal, f_lightVec));
//  vec4 texel = texture(texture, f_texture.st);
    
    vec4 diffuseReflection = vec4( diffuseFrontMaterial * diffuseLightColor * cosinus, 1.0f);
    
//  out_diffColor = texel * diffuseReflection;
    out_diffColor = clamp(diffuseReflection, 0.0, 1.0);
}