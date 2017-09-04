/*
 * random comment here
 * makes syntax highlight appaer
 * colors like springs sprouts
 */

#version 150

in float shade;
in vec3 vNorm;
in vec3 vPos;

uniform vec3 lightColor;


out vec4 out_Color;

void main(void) //Phong shading
{
    //Light intensities
    float Il = 0.8;
    float Ia = 0.1;

    //Surface/Object attributes
    float Ks = 0.5;
    float Kd = 0.5;
    float Ka = 0.5;
    float Ns = 3;
    vec3 lightPos = vec3(0.0, 5.0, 0.0);

    //Light-, reflection- and normal-vectors
    vec3 L = normalize(lightPos-vPos);
    vec3 N = normalize(vNorm);
    vec3 R = 2*dot(N,L)*N-L;
    vec3 V = normalize(-vPos);

    vec3 Idiff = lightColor * Kd * Il * dot(N,L);
    vec3 Ispec = lightColor * Ks * Il * pow(dot(R, V), Ns);
    vec3 Iamb = lightColor * Ka * Ia;

    vec3 phongShade = Ispec + Idiff + Iamb;

	out_Color=vec4(phongShade, 1.0);
}

