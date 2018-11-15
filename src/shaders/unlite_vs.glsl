#version 120

attribute vec3 position;
attribute vec3 color;

varying vec3 vColor;

void main() {
    vColor = color;
    gl_Position = gl_ModelViewProjectionMatrix * vec4(position, 1.0);
}
