#version 400
uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform mat3 N;
//uniform float B;

layout (location = 0) in vec3 vp;
layout (location = 1) in vec3 norm;
layout (location = 2) in vec2 tc;
 
out vec2 texCoord;
out vec3 viewNormals;
out vec3 viewPosition;

void main () {
 
	viewPosition = (V * M * vec4(vp,1.0)).xyz;
	texCoord = tc;
//	if(B >0.5) {
//		viewNormals =  (V*M * vec4(norm,0.0)).xyz;
//	} else {
		viewNormals =  N * norm;
//	}
	gl_Position = P * V  * M * vec4 (vp , 1.0);
}