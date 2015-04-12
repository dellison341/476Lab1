attribute vec4 aPosition;
attribute vec3 aNormal;
uniform mat4 uProjMatrix;
uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
varying vec3 vCol;
uniform vec3 uLightPos;
uniform float uDrawNormals;
uniform int uShadeModel;

uniform vec3 UaColor;
uniform vec3 UdColor;
uniform vec3 UsColor;
uniform float Ushine;
uniform vec3 uCameraLoc;

varying vec4 vPosition;
varying vec3 vLightPos;
varying vec3 vRotatedNormal;
varying vec3 vLightVec;
varying vec3 vCameraVec;

void main()
{
	vec3 rotatedNormal = normalize(uModelMatrix * vec4(aNormal, 0.0)).xyz;
	vRotatedNormal = rotatedNormal;
	vec3 lightVec = normalize(uLightPos - (uModelMatrix * aPosition).xyz);
	vLightVec = lightVec;
	vec3 cameraVec = normalize(uCameraLoc - (uViewMatrix * uModelMatrix * aPosition).xyz);
	vCameraVec = cameraVec;
	gl_Position = uProjMatrix * uViewMatrix * uModelMatrix * aPosition;

	// Draw the normals
	if (uDrawNormals > 0.0) {
		vCol = rotatedNormal;
	}
	// Gouraud
	else if (uShadeModel == 0) {

		vec3 halfNorm = normalize(lightVec + cameraVec);
		vCol = UaColor + UdColor * max(dot(lightVec, rotatedNormal), 0.0) + pow(max(dot(rotatedNormal, halfNorm), 0.0), Ushine) * UsColor;
	}
	// Full Phong
	else if (uShadeModel != 0) {
		vPosition = aPosition;
		vLightPos = uLightPos;
	}
	else {
		vCol = 0.5*(aNormal + 1.0);
	}
}
