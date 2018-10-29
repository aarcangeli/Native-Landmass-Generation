#version 120

uniform sampler2D mytex;

varying vec2 texCoord;

void main() {
    gl_FragColor = texture2D(mytex, texCoord);
}
