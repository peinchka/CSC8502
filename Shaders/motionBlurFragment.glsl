#version 450 core

uniform sampler2D diffuseTex;
uniform sampler2D diffuseTex1;
uniform sampler2D diffuseTex2;
uniform sampler2D diffuseTex3;
uniform sampler2D diffuseTex4;
uniform int motionBlurFrames;

in Vertex {
	vec4 colour;
	vec2 texCoord;
} IN;

out vec4 FragColor;

void main(void){
	if (motionBlurFrames == 1) {
		FragColor = texture(diffuseTex, IN.texCoord);
	}
	if (motionBlurFrames == 2) {
		FragColor = (texture(diffuseTex, IN.texCoord) + texture(diffuseTex1, IN.texCoord))/2.0f;
	}
	if (motionBlurFrames == 3) {
		FragColor = (texture(diffuseTex, IN.texCoord) + texture(diffuseTex1, IN.texCoord) + texture(diffuseTex2, IN.texCoord))/3.0f;
	}
	if (motionBlurFrames == 4) {
		FragColor = (texture(diffuseTex, IN.texCoord) + texture(diffuseTex1, IN.texCoord) + texture(diffuseTex2, IN.texCoord) + texture(diffuseTex3, IN.texCoord))/4.0f;
	}
	if (motionBlurFrames == 5) {
		FragColor = (texture(diffuseTex, IN.texCoord) + texture(diffuseTex1, IN.texCoord) + texture(diffuseTex2, IN.texCoord) + texture(diffuseTex3, IN.texCoord) + texture(diffuseTex4, IN.texCoord))/5.0f;
	}	
}