#version 150

in vec3 in_Position;
in vec2 in_TexCoord;

out vec2 outTextCoord;

void main() {

    outTextCoord = in_TexCoord;
	gl_Position = vec4(in_Position, 1.0);
}
