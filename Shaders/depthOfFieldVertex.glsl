#version 450 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;
uniform mat4 originalProjMatrix;

in vec4 colour;
in vec3 position;
in vec2 texCoord;

out Vertex {
	vec2 texCoord;
	mat4 inverseProj;
} OUT;

void main(void) {
	mat4 mvp = projMatrix * viewMatrix * modelMatrix;
	gl_Position = mvp * vec4(position, 1.0);
	OUT.inverseProj = inverse(originalProjMatrix);
	OUT.texCoord = (textureMatrix * vec4(texCoord, 0.0, 1.0)).xy;
}