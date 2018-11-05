#version 120

uniform sampler2D mytex;

varying vec3 positionViewSpace;
varying vec3 normalViewSpace;
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

    // triplanar mapping
    // it has been used a view space coordinates insted of the classical normal space coordinates for simplicity
    vec3 scaledWorldPos = positionViewSpace;
    vec3 blendAxes = abs(normalize(normalViewSpace));
    vec4 texColorX = texture2D(mytex, scaledWorldPos.yz) * blendAxes.x;
    vec4 texColorY = texture2D(mytex, scaledWorldPos.xz) * blendAxes.y;
    vec4 texColorZ = texture2D(mytex, scaledWorldPos.xy) * blendAxes.z;
    vec4 texColor = texColorX + texColorY + texColorZ;

    gl_FragColor = vec4(texColor.xyz * clamp(totalStrength, 0.0, 1.0), 1.0);

    //gl_FragColor = vec4(diffuseStrength, diffuseStrength, diffuseStrength, 1.0);
    //gl_FragColor = vec4(vNormal, 1.0);
    //gl_FragColor = vec4(viewPosition, 1.0);
    //gl_FragColor = vec4(normalize(normalViewSpace), 1.0);
}
