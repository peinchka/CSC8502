#version 450 core

uniform sampler2D diffuseTex;
uniform sampler2D emissiveTex;
uniform sampler2D specularTex;
uniform sampler2D skyBoxTex;

in Vertex {
	vec2 texCoord;
} IN;

out vec4 FragColor;

void main(void) {
	vec3 diffuse = texture(diffuseTex, IN.texCoord).xyz;
	vec3 light = texture(emissiveTex, IN.texCoord).xyz;
	vec3 specular = texture(specularTex, IN.texCoord).xyz;
	vec3 skybox = texture(skyBoxTex, IN.texCoord).xyz;
	
	FragColor.xyz = diffuse * 0.8; // ambient
	FragColor.xyz += diffuse * light; // lambert
	FragColor.xyz += specular; // Specular
	FragColor.xyz += skybox; // Skybox
	FragColor.a = 1.0;
//FragColor = vec4(1,0,0,1);
}