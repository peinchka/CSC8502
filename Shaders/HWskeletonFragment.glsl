#version 330 core

uniform sampler2D diffuseTex;

in Vertex {
	vec4    colour;
	vec2    texCoord;
	vec3 	normal;
	vec3 	tangent;
	vec3 	worldPos;
} IN;

out vec4 FragColor;

void main(void){
	FragColor = texture(diffuseTex, IN.texCoord);
}