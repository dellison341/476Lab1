varying vec3 vCol;

uniform float uDrawNormals;
uniform int uShadeModel;

uniform vec3 UaColor;
uniform vec3 UdColor;
uniform vec3 UsColor;
uniform float Ushine;

varying vec4 vPosition;
varying vec3 vLightPos;
varying vec3 vRotatedNormal;
varying vec3 vLightVec;
varying vec3 vCameraVec;


void main()
{
	if (uDrawNormals > 0.0) {
		gl_FragColor = vec4(vCol.r, vCol.g, vCol.b, 1.0);
	}
	// Gouraud
	else if (uShadeModel == 0) {
		gl_FragColor = vec4(vCol.r, vCol.g, vCol.b, 1.0);
	}
	// Full Phong
	else if (uShadeModel != 0) {
		vec3 halfNorm = normalize(vLightVec + vCameraVec);
		vec3 fragColor = UaColor + UdColor * max(dot(normalize(vLightVec), vRotatedNormal), 0.0) + pow(max(dot(vRotatedNormal, halfNorm), 0.0), Ushine) * UsColor;

		gl_FragColor = vec4(fragColor, 1.0);
	}
	else {
		gl_FragColor = vec4(vCol.r, vCol.g, vCol.b, 1.0);
	}
}
