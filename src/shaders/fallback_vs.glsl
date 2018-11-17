#version 330

layout(location = 0) attribute vec3 position;
layout(location = 1) attribute vec3 normal;
layout(location = 2) attribute vec3 color;
layout(location = 3) attribute vec3 uv;

uniform mat4 projMat;
uniform mat4 viewMat;
uniform mat4 modelMat;

void main() {
    gl_Position = projMat * viewMat * modelMat * vec4(position, 1.0);
}
