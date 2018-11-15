#version 120

#define MAX_LAYERS 8

varying vec3 vColor;

void main() {
    gl_FragColor = vec4(vColor, 1.0);
}
