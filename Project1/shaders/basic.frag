#version 330 core

uniform sampler2D ourTexture1;
in vec2 texCoord;

out vec4 color;

void main()
{    
    color = texture(ourTexture1, texCoord);
}
  