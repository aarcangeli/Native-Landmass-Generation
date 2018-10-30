#version 120

uniform sampler2D mytex;

varying vec2 texCoord;
varying vec3 color;

void main() {
    gl_FragColor = texture2D(mytex, texCoord);
    gl_FragColor = vec4(color, 0.0);
}
