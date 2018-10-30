#version 120

attribute vec3 position;
attribute vec3 normal;
attribute vec3 uv;

varying vec2 texCoord;
varying vec3 vNormal;
varying vec3 normalDiff;

void main() {
    gl_Position = gl_ModelViewProjectionMatrix * vec4(position, 1.0);
    texCoord = uv.xy;
    vNormal = normalize(normal);
    // vertex position in camera space coordinates
    vec3 positionCameraSpace = vec3(gl_ModelViewMatrix * vec4(position, 1.0));
    // normal "arrow tip position" in camera space coordinates
    vec3 normalCameraSpace = vec3(gl_ModelViewMatrix * vec4(position + normalize(normal), 1.0));
    // normal direction in camera space coordinates
    normalDiff = normalCameraSpace - positionCameraSpace;
}
