#version 400
uniform float triangleColor;
uniform mat4 trans;
uniform mat4 view;
uniform mat4 proj;
layout (location = 0) in vec3 vp;
layout (location = 1) in vec3 vc;
layout (location = 2) in vec2 tc;

out vec3 position;
out vec3 color;
out vec2 texCoord;

void main () {
	position = vp;
	color = vc;
	texCoord = tc;
	gl_Position =   proj * view  * trans * vec4 (vp , 1.0);
}