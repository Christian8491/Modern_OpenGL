#version 150 core

in vec3 vPosition;
in vec2 vTexCoord; // other shader ?

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoord;

void main() { 
    
    gl_Position =  projection * view * model * vec4(vPosition, 1.0); 
    TexCoord = vTexCoord;
}