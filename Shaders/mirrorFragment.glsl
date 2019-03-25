#version 450 core

uniform samplerCube cubeTex;

uniform vec4 lightColour;
uniform vec3 lightPos;
uniform vec3 cameraPos;
uniform float lightRadius;

in Vertex {
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 worldPos;
} IN;

out vec4 gl_FragColor;

void main(void) {
	vec3 incident = normalize(IN.worldPos - cameraPos );
	float dist = length(lightPos - IN.worldPos);
	float atten = 1.0 - clamp(dist/lightRadius, 0.0, 1.0);
	vec4 reflection = texture(cubeTex, reflect(incident, normalize(IN.normal)));
	gl_FragColor = vec4(reflection.xyz, 0.7);
}