#version 150 core

in vec2 TexCoord;

uniform vec3 backgroundColor;
uniform sampler2D ourTexture;

out vec4 fColor;

void main() { 
	
	if (TexCoord == 1)  fColor = texture(ourTexture, TexCoord);
	else fColor =  vec4(backgroundColor, 1.0); 
}