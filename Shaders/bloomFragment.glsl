#version 450

uniform sampler2D diffuseTex;
uniform vec2 pixelSize;
uniform int isVertical;

in Vertex {
	vec4 colour;
	vec2 texCoord;
} IN;

out vec4 FragColor;

const float weights[9] = float[](0.028532, 0.067234, 0.124009, 0.179044, 0.20236, 0.179044, 0.124009, 0.067234, 0.028532); //Using Sigma = 2 and Kernel = 9 @ http://dev.theomader.com/gaussian-kernel-calculator/

void main(void) {
	vec2 values[9];
	if(isVertical == 1) {
		values = vec2[](vec2(0.0, - pixelSize.y*4), vec2(0.0, - pixelSize.y*3), vec2(0.0, - pixelSize.y*2), vec2(0.0, - pixelSize.y), vec2(0.0, 0.0), vec2(0.0, pixelSize.y), vec2(0.0, pixelSize.y*2), vec2(0.0, pixelSize.y*3), vec2(0.0, pixelSize.y*4));
	}
	else {
		values = vec2[](vec2(- pixelSize.x*4, 0.0), vec2(- pixelSize.x*3, 0.0), vec2(- pixelSize.x*2, 0.0), vec2(- pixelSize.x, 0.0), vec2(0.0, 0.0), vec2(pixelSize.x, 0.0), vec2(pixelSize.x*2, 0.0), vec2(pixelSize.x*3, 0.0), vec2(pixelSize.x*4, 0.0));
	}
	for (int i = 0; i < 9; i++) {
		vec4 temp = texture2D(diffuseTex, IN.texCoord.xy + values[i]);
		FragColor += temp * weights[i];
	}
}


