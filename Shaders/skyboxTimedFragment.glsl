#version 450 core

uniform samplerCube cubeTex;
uniform vec3 cameraPos;
uniform float time;

in Vertex {
	vec3 normal;
} IN;

out vec4 FragColor;

void main(void) {
	FragColor = texture(cubeTex, normalize(IN.normal)) * (0.8 * pow(cos(0.1 * time), 2) + 0.2);
}