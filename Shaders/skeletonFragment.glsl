#version 150

uniform sampler2D diffuseTex;

in Vertex {
	vec4 colour;
	vec2 texCoord;
} IN;

out vec4 FragColor;

void main(void){
	FragColor = texture(diffuseTex, IN.texCoord);
}