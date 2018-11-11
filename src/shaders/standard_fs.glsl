#version 120

#define MAX_LAYERS 8

uniform int layerCount;

uniform sampler2D layerText[MAX_LAYERS];
// packed startHeight, blend, textureScale
uniform vec3 layerPackList[MAX_LAYERS];
// packed minHeight, maxHeight
uniform vec2 heightRange;

varying vec3 positionViewSpace;
varying vec3 normalViewSpace;
varying vec2 texCoord;
varying vec3 vNormal;
varying vec3 positionCameraSpace;
varying vec3 normalCameraSpace;
varying vec3 normalDiff;

float saturate(float num) {
    return clamp(num, 0.0, 1.0);
}
float inverseLerp(float a, float b, float value) {
    return saturate((value - a)/(1E-4 + b - a));
}

vec4 triplanar(sampler2D texture, vec3 coord, vec3 normal) {
    vec3 blendAxes = abs(normalize(normal));
    vec4 texColorX = texture2D(texture, coord.yz) * blendAxes.x;
    vec4 texColorY = texture2D(texture, coord.xz) * blendAxes.y;
    vec4 texColorZ = texture2D(texture, coord.xy) * blendAxes.z;
    return (texColorX + texColorY + texColorZ) / (blendAxes.x + blendAxes.y + blendAxes.z);
}

void main() {
    // color
    vec4 texColor = vec4(1.);
    float minHeight = heightRange.x;
    float maxHeight = heightRange.y;
    float heightPercent = inverseLerp(minHeight, maxHeight, positionViewSpace.y);
    for (int i = layerCount - 1; i >= 0; i--) {
        float baseStartHeights = layerPackList[i].x;
        float baseBlends = layerPackList[i].y;
        float textureScale = layerPackList[i].z;
        float drawStrength = 1. - inverseLerp(baseStartHeights - baseBlends / 2, baseStartHeights + baseBlends / 2, heightPercent);

        vec4 textureColour = triplanar(layerText[i], positionViewSpace * textureScale, normalViewSpace);
        texColor = texColor * (1 - drawStrength) + textureColour * drawStrength;
    }

    // ambient lightning
    float ambientStrength = 0.5;

    // diffuse lightning located at camera position
    float diffuseStrength = dot(vec3(0.0, 0.0, 1.0), normalize(normalDiff));
    diffuseStrength = max(diffuseStrength, 0.0);

    float totalStrength = ambientStrength + diffuseStrength / 2.0;


    gl_FragColor = vec4(texColor.xyz * saturate(totalStrength), 1.0);
    //gl_FragColor = texture2D(layerText[1], positionViewSpace.xy);

    //gl_FragColor = vec4(diffuseStrength, diffuseStrength, diffuseStrength, 1.0);
    //gl_FragColor = vec4(vNormal, 1.0);
    //gl_FragColor = vec4(viewPosition, 1.0);
    //gl_FragColor = vec4(normalize(normalViewSpace), 1.0);
}
