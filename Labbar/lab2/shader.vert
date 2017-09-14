#version 150

//in vec3 in_Color;
in vec3 in_Position;
in vec3 in_Normal;
in vec2 in_TexCoord;
uniform mat4 matrix;

uniform mat4 mBenRot1;
uniform mat4 mBenRot2;
uniform mat4 mBenPos1;
uniform mat4 mBenPos2;

out vec4 g_color;
const vec3 lightDir = normalize(vec3(0.3, 0.5, 1.0));

// Uppgift 3: Soft-skinning på GPU
//
// Flytta över din implementation av soft skinning från CPU-sidan
// till vertexshadern. Mer info finns på hemsidan.

void main(void)
{

    mat4 mBen1, mBen2;

    mBen1 = mBenRot1 * mBenPos1;
    mBen2 = mBenRot2 * mBenPos2;

    // v'_m = (w_1 * M_ben1 * v_1) + (w_2 * M_ben2 * v_2)
    vec4 wMv1 = in_TexCoord.x * mBen1 * vec4(in_Position, 1.0);
    vec4 wMv2 = in_TexCoord.y * mBen2 * vec4(in_Position, 1.0);

    vec4 vPrim = (wMv1 + wMv2);

    // transformera resultatet med ModelView- och Projection-matriserna
	gl_Position =  matrix * vPrim;
//	gl_Position =  matrix * vec4(in_Position, 1.0);

	// sätt röd+grön färgkanal till vertex Weights
	vec4 color = vec4(in_TexCoord.x, in_TexCoord.y, 0.0, 1.0);

	// Lägg på en enkel ljussättning på vertexarna
	float intensity = dot(in_Normal, lightDir);
	color.xyz *= intensity;

	g_color = color;
}

