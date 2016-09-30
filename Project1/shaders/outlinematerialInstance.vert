#version 420
//uniform mat4 M;
//uniform mat4 V;
//uniform mat4 P;
//uniform mat3 N;
//uniform float B;

layout(std140, binding = 0) uniform Matrices 
{
	mat4 V;
	mat4 P;
};


layout (location = 0) in vec3 vp;
layout (location = 1) in vec3 norm;
layout (location = 2) in vec2 tc;
layout (location = 3) in mat4 M;
layout (location = 7) in mat4 N;

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