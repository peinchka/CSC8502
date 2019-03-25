#version 450

uniform sampler2D diffuseTex;
uniform int bloomInfluence;
uniform bool bloomPerColour;

in Vertex {
	vec4 colour;
	vec2 texCoord;
} IN;

out vec4 FragColor;

void main(void) {
	vec4 temp = texture2D(diffuseTex, IN.texCoord.xy);
	if (bloomPerColour) {
		if (temp.r > bloomInfluence/300.0f || temp.g > bloomInfluence/300.0f || temp.b > bloomInfluence/300.0f) {
			FragColor = temp;
		}
		else {
			FragColor = vec4(0, 0, 0, 0);
		}
	}
	else {
		if (temp.r + temp.g + temp.r > bloomInfluence/100.0f) {
			FragColor = temp;
		}
		else {
			FragColor = vec4(0, 0, 0, 0);
		}

	}	
}


