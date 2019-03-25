#version 450

uniform sampler2D diffuseTex;
uniform vec2 pixelSize;
uniform int isVertical;

in Vertex {
	vec4 colour;
	vec2 texCoord;
	vec3 worldPos;
} IN;

out vec4 FragColor;

const float weights[5] = float[](0.12, 0.22, 0.32, 0.22, 0.12);

void main(void) {

	vec4 sobelx = vec4(0.0, 0.0, 0.0, 1.0);
	vec4 tmpx = texture2D(diffuseTex, IN.texCoord.xy + vec2(-pixelSize.x, -pixelSize.y)) * -1.0;
	sobelx += tmpx;
	tmpx = texture2D(diffuseTex, IN.texCoord.xy + vec2(-pixelSize.x, 0)) * -2.0;
	sobelx += tmpx;
	tmpx = texture2D(diffuseTex, IN.texCoord.xy + vec2(-pixelSize.x, pixelSize.y)) * -1.0;
	sobelx += tmpx;
	tmpx = texture2D(diffuseTex, IN.texCoord.xy + vec2(pixelSize.x, -pixelSize.y));
	sobelx += tmpx;
	tmpx = texture2D(diffuseTex, IN.texCoord.xy + vec2(pixelSize.x, 0)) * 2.0;
	sobelx += tmpx;
	tmpx = texture2D(diffuseTex, IN.texCoord.xy + vec2(pixelSize.x, pixelSize.y));
	sobelx += tmpx;

	vec4 sobely = vec4(0.0, 0.0, 0.0, 1.0);
	vec4 tmpy = texture2D(diffuseTex, IN.texCoord.xy + vec2(-pixelSize.x, pixelSize.y)) * -1.0;
	sobely += tmpy;
	tmpy = texture2D(diffuseTex, IN.texCoord.xy + vec2(0, pixelSize.y)) * -2.0;
	sobely += tmpy;
	tmpy = texture2D(diffuseTex, IN.texCoord.xy + vec2(pixelSize.x, pixelSize.y)) * -1.0;
	sobely += tmpy;
	tmpy = texture2D(diffuseTex, IN.texCoord.xy + vec2(-pixelSize.x, -pixelSize.y));
	sobely += tmpy;
	tmpy = texture2D(diffuseTex, IN.texCoord.xy + vec2(0, -pixelSize.y)) * 2.0;
	sobely += tmpy;
	tmpy = texture2D(diffuseTex, IN.texCoord.xy + vec2(pixelSize.x, -pixelSize.y));
	sobely += tmpy;

	if (sobelx.r > 1.0) {
		sobelx.r = 1.0;
	}
	if (sobelx.g > 1.0) {
		sobelx.g = 1.0;
	}
	if (sobelx.b > 1.0) {
		sobelx.b = 1.0;
	}
	sobelx.a = 1.0;

	if (sobely.r > 1.0) {
		sobely.r = 1.0;
	}
	if (sobely.g > 1.0) {
		sobely.g = 1.0;
	}
	if (sobely.b > 1.0) {
		sobely.b = 1.0;
	}
	sobely.a = 1.0;

	vec4 sobel = sqrt(sobelx*sobelx + sobely*sobely);

	if ((sobel.r > 0.8) || (sobel.g > 0.8) || (sobel.b > 0.8)) {
		vec2 values[5];
		if(isVertical == 1) {
			values = vec2[](vec2(0.0, - pixelSize.y*2), vec2(0.0, - pixelSize.y), vec2(0.0, 0.0), vec2(0.0, pixelSize.y), vec2(0.0, pixelSize.y*2));
		}
		else {
			values = vec2[](vec2(- pixelSize.x*2, 0.0), vec2(- pixelSize.x, 0.0), vec2(0.0, 0.0), vec2(pixelSize.x, 0.0), vec2(pixelSize.x*2, 0.0));
		}

		for (int i = 0; i < 5; i++) {
			vec4 tmp = texture2D(diffuseTex, IN.texCoord.xy + values[i]);
			FragColor += tmp * weights[i];
		}
	}
	else {
		FragColor = texture2D(diffuseTex, IN.texCoord);
	}
}

