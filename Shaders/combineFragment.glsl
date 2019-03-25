#version 450 core

uniform sampler2D diffuseTex;
uniform sampler2D diffuseTex1;

in Vertex {
	vec4 colour;
	vec2 texCoord;
} IN;

out vec4 FragColor;

void main(void){
	FragColor = texture(diffuseTex, IN.texCoord) + texture(diffuseTex1, IN.texCoord);
}