#version 450 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 shadowMatrix;
uniform mat4 shadowMatrix1;
uniform mat4 shadowMatrix2;
uniform mat4 shadowMatrix3;
uniform mat4 shadowMatrix4;
uniform mat4 shadowMatrix5;
uniform mat4 shadowMatrix10;

in vec3 position;
in vec3 colour;
in vec3 normal;
in vec3 tangent;
in vec2 texCoord;

out Vertex {
 	vec3 colour;
 	vec2 texCoord;
 	vec3 normal;
 	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
 	vec4 shadowProj;
 	vec4 shadowProj1;
	vec4 shadowProj2;
	vec4 shadowProj3;
	vec4 shadowProj4;
	vec4 shadowProj5;
	vec4 shadowProj10;
} OUT;

void main(void) {
	mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
 	OUT.colour = colour;
 	OUT.texCoord = texCoord;
 	OUT.normal = normalize(normalMatrix * normalize(normal));
 	OUT.tangent = normalize(normalMatrix * normalize(tangent));
 	OUT.binormal = normalize(normalMatrix * normalize(cross(normal, tangent)));
 	OUT.worldPos = (modelMatrix * vec4(position, 1)).xyz;
 	OUT.shadowProj = (shadowMatrix * vec4(position + (normal * 1.5), 1.0));
 	OUT.shadowProj1 = (shadowMatrix1 * vec4(position + (normal * 1.5), 1.0));
	OUT.shadowProj2 = (shadowMatrix2 * vec4(position + (normal * 1.5), 1.0));
	OUT.shadowProj3 = (shadowMatrix3 * vec4(position + (normal * 1.5), 1.0));
	OUT.shadowProj4 = (shadowMatrix4 * vec4(position + (normal * 1.5), 1.0));
	OUT.shadowProj5 = (shadowMatrix5 * vec4(position + (normal * 1.5), 1.0));
	OUT.shadowProj10 = (shadowMatrix10 * vec4(position , 1.0));
 	gl_Position = (projMatrix * viewMatrix * modelMatrix) * vec4(position, 1.0);
}
