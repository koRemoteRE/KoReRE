#version 150

in vec3 f_lightVec;
in vec3 f_normal;
in vec3 f_texture;

uniform float textureCount;
uniform sampler2D texture2D;
uniform vec3 diffuseLightColor;
uniform vec4 diffuseMaterialColor;

out vec4 out_diffColor;

void main()
{
    float cosinus = max(0.0f, dot(f_normal, f_lightVec));
    
    //vec4 texel = texture(texture, f_texture.st);
    
    vec4 diffuseReflection = vec4( diffuseMaterialColor.rgb * diffuseLightColor * cosinus, 1.0f);
    
    //    out_diffColor = texel * diffuseReflection;
    //if (textureCount == 0)
    //out_diffColor = vec4(f_texture,1.0f);
    out_diffColor = clamp(diffuseReflection, 0.0, 1.0);
}