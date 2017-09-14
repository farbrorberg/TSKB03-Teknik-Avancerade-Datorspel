#version 150
// bump mapping should be calculated
// 1) in view coordinates
// 2) in texture coordinates

in vec2 outTexCoord;
in vec3 out_Normal;
in vec3 Ps;
in vec3 Pt;
in vec3 pixPos;  // Needed for specular reflections
uniform sampler2D texUnit;
uniform float factor;
out vec4 out_Color;

void main(void)
{
    vec3 light = vec3(0.0, 0.7, 0.7); // Light source in view coordinates
	
	// Calculate gradients here
	float offset = 1.0 / 256.0; // texture size, same in both directions
	vec2 tc = outTexCoord;


    /*
    *   VY-KOORDINATED
    */
	// bs = b[s+1, t] - b[s, t]
    // bt = b[s, t+1] - b[s, t]
//    vec3 bs = texture(texUnit, tc + vec2(offset, 0.0)).xyz - texture(texUnit, tc).xyz;
//    vec3 bt = texture(texUnit, tc + vec2(0.0, offset)).xyz - texture(texUnit, tc).xyz;
//
//
//    // n' = n +- bs*Ps +- bt*Pt
//    vec3 normal = normalize(out_Normal + factor*(bs*Ps) + factor*(bt*Pt));
//
//	out_Color = vec4( dot(normal, light));


	/*tat
    *   TEXTUR-KOORDINATED
    */

    vec4 ds = texture(texUnit, vec2(tc.x+offset, tc.y)) - texture(texUnit, vec2(tc.x-offset, tc.y));
    vec4 dt = texture(texUnit, vec2(tc.x, tc.y+offset)) - texture(texUnit, vec2(tc.x, tc.y-offset));

    //Mvt matrix - for converting into texture coordinates
    mat3 Mvt = transpose(mat3(Ps, Pt, out_Normal));

    // n' = n + ds*Ps + dt*Pt - Är detta korrekt?
    vec3 normal = normalize(out_Normal + ds.xyz*Ps + dt.xyz*Pt);
    //Eller är detta korrekt?
//    vec3 normal = normalize(vec3(ds.x, ds.y, 1.0));

    //In texture coordinate space
    out_Color = vec4( dot(normal, Mvt*light));

}
