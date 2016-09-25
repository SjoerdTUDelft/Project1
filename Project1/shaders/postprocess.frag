#version 400

uniform sampler2D ourTexture1;

in vec2 texCoord;
 
 
out vec4 frag_colour;
 
void main () {
	vec3 endcol = texture2D(ourTexture1,texCoord).xyz;
	float avg = 0.2126 * endcol.r + 0.7152 * endcol.g + 0.0722 * endcol.b;
 	frag_colour = vec4(avg,avg,avg,1.0);
	 
}