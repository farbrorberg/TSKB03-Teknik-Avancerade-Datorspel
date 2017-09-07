#version 150

in vec2 outTexCoord;

uniform sampler2D texUnit;
uniform sampler2D texUnit2;

out vec4 outColor;

void main(void)
{
    outColor = 0.6*texture(texUnit, outTexCoord) + 1.0*texture(texUnit2, outTexCoord);
}
