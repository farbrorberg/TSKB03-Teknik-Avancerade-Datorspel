#version 150

in  vec3  in_Position;
in  vec3  in_Normal;
in  vec2  in_TexCoord;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

out float shade;
out vec3 vNorm;
out vec3 vPos;

void main() {


	gl_Position = vec4(vec3(0.0), 1.0);
}
