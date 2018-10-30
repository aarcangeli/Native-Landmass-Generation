#version 120

uniform sampler2D mytex;

varying vec2 texCoord;
varying vec3 vNormal;
varying vec3 positionCameraSpace;
varying vec3 normalCameraSpace;
varying vec3 normalDiff;

void main() {
    // ambient lightning
    float ambientStrength = 0.5;

    // diffuse lightning located at camera position
    float diffuseStrength = dot(vec3(0.0, 0.0, 1.0), normalize(normalDiff));
    diffuseStrength = max(diffuseStrength, 0.0);

    float totalStrength = ambientStrength + diffuseStrength / 2.0;

    vec3 texColor = texture2D(mytex, texCoord).xyz;

    gl_FragColor = vec4(texColor * clamp(totalStrength, 0.0, 1.0), 1.0);

    //gl_FragColor = vec4(diffuseStrength, diffuseStrength, diffuseStrength, 1.0);
    //gl_FragColor = vec4(vNormal, 1.0);
}
