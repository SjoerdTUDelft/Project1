#version 400

uniform sampler2D ourTexture1;
uniform sampler2D ourTexture2;
uniform float triangleColor;


in vec3 position;
in vec3 color;
in vec2 texCoord;

out vec4 frag_colour;

void main () {
	vec2 TexCoord = vec2(texCoord.x,1.0-texCoord.y);
  //frag_colour = vec4(triangleColor,1.0);
  vec4 tex1 = texture(ourTexture1,TexCoord);
  vec4 tex2 = texture(ourTexture2,TexCoord);
  frag_colour = mix(tex1,tex2,triangleColor);
}