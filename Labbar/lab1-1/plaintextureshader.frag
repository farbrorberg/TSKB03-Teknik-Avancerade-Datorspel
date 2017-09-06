
#version 150

in vec2 outTexCoord;

uniform sampler2D texUnit;
uniform sampler2D object;
uniform sampler2D bloom;

out vec4 out_Color;

void main(void)
{
    out_Color = texture(object, outTexCoord) + texture(bloom, outTexCoord);
}

