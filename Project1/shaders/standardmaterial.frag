#version 400

uniform sampler2D ourTexture1;
  
in vec3 position;
in vec2 texCoord;
in vec3 worldNormals;

out vec4 frag_colour;

void main () {
	float NdL = dot(worldNormals,lightPos)
	vec4 diffuse = texture(ourTexture1,texCoord);
	frag_colour = 
}