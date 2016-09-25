#version 330 core

layout (location = 0) in vec3 vp;
layout (location = 2) in vec2 tc;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

out vec2 texCoord;

void main()
{    
	texCoord = tc;
	gl_Position = P * V  * M * vec4 (vp , 1.0);
}
  