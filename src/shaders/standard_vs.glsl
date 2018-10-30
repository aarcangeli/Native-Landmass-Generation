#version 120

attribute vec3 position;
attribute vec3 normal;
attribute vec3 uv;

varying vec2 texCoord;
varying vec3 color;

void main() {
    gl_Position = gl_ModelViewProjectionMatrix * vec4(position, 1.0);
    texCoord = uv.xy;
    color = normalize(normal);
}
