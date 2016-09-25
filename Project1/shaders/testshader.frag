#version 400

//uniform sampler2D ourTexture1;
//uniform sampler2D ourTexture2;
//uniform float triangleColor;

in vec2 texCoord;
in vec3 viewNormals;
in vec3 viewPosition;
in vec3 lightViewPosition;

 

out vec4 frag_colour;

void main () {
  //vec2 TexCoord = vec2(texCoord.x, texCoord.y);
  //frag_colour = vec4(triangleColor,1.0);
  //vec4 tex1 = texture(ourTexture1,TexCoord);
  //vec4 tex2 = texture(ourTexture2,TexCoord);
  //frag_colour = mix(tex1,tex2,0.5);
  //frag_colour = vec4(texCoord,0.0,1.0);
	frag_colour = vec4(1.0,0.55,0.0,1.0);

}