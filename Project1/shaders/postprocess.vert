#version 400

layout (location = 0) in vec3 vp;
layout (location = 2) in vec2 tc;

out vec2 texCoord;
 

void main() {
	texCoord = tc;
	gl_Position = vec4(vp.xy,0.0,1.0);
}