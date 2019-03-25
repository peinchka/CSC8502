#version 450

uniform sampler2D diffuseTex;
uniform vec2 pixelSize;
uniform int doubleVision;

in Vertex {
	vec4 colour;
	vec2 texCoord;
} IN;

out vec4 FragColor;

void main(void) {
	FragColor = (texture2D(diffuseTex, IN.texCoord) + texture2D(diffuseTex, vec2(IN.texCoord.x * (1/pixelSize.x - doubleVision) * pixelSize.x, IN.texCoord.y * (1/pixelSize.y - doubleVision) * pixelSize.y) + vec2(pixelSize.x * doubleVision, pixelSize.y * doubleVision)))/2.0;
}

