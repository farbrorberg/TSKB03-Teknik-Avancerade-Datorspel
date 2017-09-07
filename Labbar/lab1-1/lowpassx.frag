#version 150

uniform sampler2D texUnit;
in vec2 outTexCoord;
out vec4 outColor;

void main(void)
{
    vec3 sum = vec3(0.0);

    //Radius / resolution (?)
	float blur = 1.0 / 256;

    sum += texture(texUnit, outTexCoord - vec2(blur, 0.0)).rgb;
    sum += 2.0 * texture(texUnit, outTexCoord).rgb;
    sum += texture(texUnit, outTexCoord + vec2(blur, 0.0)).rgb;
    sum /= 4;

    outColor = vec4(sum, 1.0);
}