#version 150

in vec2 outTexCoord;

uniform sampler2D texUnit;

out vec4 outcolor;

void main() {

    vec4 col = texture(texUnit, outTexCoord);
    outcolor = max(col - 1.0, 0.0); // Saving truncated values above 1.0
}
