#version 150

in vec2 outTextCoord;

uniform sampler2D texUnit;

out vec4 out_color;

void main() {

    vec4 col = texture(texUnit, outTextCoord);
    out_color = max(col - 1.0, 0.0); // Saving truncated values above 1.0
}
