#version 120

attribute vec3 position;
attribute vec3 uv;

varying vec2 texCoord;

void main() {
    gl_Position = gl_ModelViewProjectionMatrix * vec4(position, 1.0);
    texCoord = uv.xy;
}
