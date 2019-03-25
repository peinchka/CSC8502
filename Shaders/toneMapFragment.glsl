#version 450 core

uniform sampler2D diffuseTex;
uniform float toneMap;
uniform bool redGreenSwap;
uniform bool redBlueSwap;
uniform bool greenBlueSwap;
uniform bool noMinSwap;
uniform bool noMaxSwap;

in Vertex {
	vec4 colour;
	vec2 texCoord;
} IN;

out vec4 FragColor;

void main(void){
	float min = toneMap/500.0;
	float max = toneMap/500.0;
	FragColor = texture(diffuseTex, IN.texCoord);
	if (FragColor.r <= min) {
		if (!noMinSwap) {
			FragColor.r = 0.0;
		}		
	}
	if (FragColor.r > max) {
 		if (!noMaxSwap) {
			FragColor.r = 1.0;
		}
	}
	if (FragColor.g <= min) {
 		if (!noMinSwap) {
			FragColor.g = 0.0;
		}
	}
	if (FragColor.g > max) {
 		if (!noMaxSwap) {
			FragColor.g = 1.0;
		}
	}
	if (FragColor.b <= min) {
 		if (!noMinSwap) {
			FragColor.b = 0.0;
		}
	}
	if (FragColor.b > max) {
 		if (!noMaxSwap) {
			FragColor.b = 1.0;
		}
	}

	if (redGreenSwap) {
		FragColor = FragColor.grba;
	}
	if (redBlueSwap) {
		FragColor = FragColor.bgra;
	}
	if (greenBlueSwap) {
		FragColor = FragColor.rbga;
	}
}