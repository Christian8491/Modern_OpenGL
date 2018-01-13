#version 150 core

in vec2 TexCoord;

uniform sampler2D ourTexture;

out vec4 fColor;

void main() { 
	
	fColor = texture(ourTexture, TexCoord);
}