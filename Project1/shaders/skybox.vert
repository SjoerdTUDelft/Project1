#version 330 core
layout (location = 0) in vec3 position;
out vec3 TexCoords;

uniform mat4 P;
uniform mat4 V;


void main()
{
   // gl_Position =   P * V * vec4(200.0* position, 1.0);  
    //TexCoords = position;


	    vec4 pos = P * V * vec4(position, 1.0);
		
    gl_Position = pos.xyww;
	gl_Position.z *= 0.9999995;
    TexCoords = position;
} 