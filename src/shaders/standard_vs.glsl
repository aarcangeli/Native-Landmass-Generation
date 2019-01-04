#version 330
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) attribute vec3 position;
layout(location = 1) attribute vec3 normal;
layout(location = 2) attribute vec3 color;
layout(location = 3) attribute vec3 uv;

uniform mat4 projMat;
uniform mat4 viewMat;
uniform mat4 modelMat;

varying vec3 positionViewSpace;
varying vec3 normalViewSpace;
varying vec2 texCoord;
varying vec3 vNormal;
varying vec3 normalDiff;

void main() {
    positionViewSpace = position;
    normalViewSpace = normalize(normal);
    gl_Position = projMat * viewMat * modelMat * vec4(position, 1.0);
    texCoord = uv.xy;
    vNormal = normalize(normal);
    // vertex position in camera space coordinates
    vec3 positionCameraSpace = vec3(viewMat * modelMat * vec4(position, 1.0));
    // normal "arrow tip position" in camera space coordinates
    vec3 normalCameraSpace = vec3(viewMat * modelMat * vec4(position + normalize(normal), 1.0));
    // normal direction in camera space coordinates
    normalDiff = normalCameraSpace - positionCameraSpace;
}
