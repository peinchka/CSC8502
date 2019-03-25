#version 450

uniform sampler2D diffuseTex;
uniform vec2 pixelSize;
uniform int isVertical;

in Vertex {
	vec4 colour;
	vec2 texCoord;
} IN;

out vec4 gl_FragColor;
//Technically , this isn't quite a 'gaussian' distribution...
const float weights[5] = float[](0.12, 0.22, 0.32, 0.22, 0.12);

void main(void) {

//	vec4 tmp = texture2D(diffuseTex, IN.texCoord);
//	float gs = (tmpx.r + tmpx.g + tmpx.b)/3.0;
//	vec4 tmp = vec4(gs, gs, gs, 1);

	vec4 sobelx = vec4(0.0,0.0,0.0,1.0);
	vec4 tmpx = texture2D(diffuseTex, IN.texCoord.xy + vec2(-pixelSize.x, -pixelSize.y)) * -1.0;
	float gs = (tmpx.r + tmpx.g + tmpx.b)/3.0;
	vec4 tmp = vec4(gs, gs, gs, 1);
	sobelx += tmp;
	tmpx = texture2D(diffuseTex, IN.texCoord.xy + vec2(-pixelSize.x, 0)) * -2.0;
	gs = (tmpx.r + tmpx.g + tmpx.b)/3.0;
	tmp = vec4(gs, gs, gs, 1);
	sobelx += tmp;
	tmpx = texture2D(diffuseTex, IN.texCoord.xy + vec2(-pixelSize.x, pixelSize.y)) * -1.0;
	gs = (tmpx.r + tmpx.g + tmpx.b)/3.0;
	tmp = vec4(gs, gs, gs, 1);
	sobelx += tmp;
	tmpx = texture2D(diffuseTex, IN.texCoord.xy + vec2(pixelSize.x, -pixelSize.y));
	gs = (tmpx.r + tmpx.g + tmpx.b)/3.0;
	tmp = vec4(gs, gs, gs, 1);
	sobelx += tmp;
	tmpx = texture2D(diffuseTex, IN.texCoord.xy + vec2(pixelSize.x, 0)) * 2.0;
	gs = (tmpx.r + tmpx.g + tmpx.b)/3.0;
	tmp = vec4(gs, gs, gs, 1);
	sobelx += tmp;
	tmpx = texture2D(diffuseTex, IN.texCoord.xy + vec2(pixelSize.x, pixelSize.y));
	gs = (tmpx.r + tmpx.g + tmpx.b)/3.0;
	tmp = vec4(gs, gs, gs, 1);
	sobelx += tmp;

	vec4 sobely = vec4(0.0,0.0,0.0,1.0);
	vec4 tmpy = texture2D(diffuseTex, IN.texCoord.xy + vec2(-pixelSize.x, pixelSize.y)) * -1.0;
	gs = (tmpy.r + tmpy.g + tmpy.b)/3.0;
	tmp = vec4(gs, gs, gs, 1);
	sobely += tmp;
	tmpy = texture2D(diffuseTex, IN.texCoord.xy + vec2(0, pixelSize.y)) * -2.0;
	gs = (tmpy.r + tmpy.g + tmpy.b)/3.0;
	tmp = vec4(gs, gs, gs, 1);
	sobely += tmp;
	tmpy = texture2D(diffuseTex, IN.texCoord.xy + vec2(pixelSize.x, pixelSize.y)) * -1.0;
	gs = (tmpy.r + tmpy.g + tmpy.b)/3.0;
	tmp = vec4(gs, gs, gs, 1);
	sobely += tmp;
	tmpy = texture2D(diffuseTex, IN.texCoord.xy + vec2(-pixelSize.x, -pixelSize.y));
	gs = (tmpy.r + tmpy.g + tmpy.b)/3.0;
	tmp = vec4(gs, gs, gs, 1);
	sobely += tmp;
	tmpy = texture2D(diffuseTex, IN.texCoord.xy + vec2(0, -pixelSize.y)) * 2.0;
	gs = (tmpy.r + tmpy.g + tmpy.b)/3.0;
	tmp = vec4(gs, gs, gs, 1);
	sobely += tmp;
	tmpy = texture2D(diffuseTex, IN.texCoord.xy + vec2(pixelSize.x, -pixelSize.y));
	gs = (tmpy.r + tmpy.g + tmpy.b)/3.0;
	tmp = vec4(gs, gs, gs, 1);
	sobely += tmp;

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

//	vec4 sobel = sqrt(sobelx*sobelx + sobely*sobely);

	gl_FragColor = sqrt(sobelx*sobelx + sobely*sobely);

/*	if ((sobel.r > 0.8) || (sobel.g > 0.8) || (sobel.b > 0.8)) {
		vec2 values[5];
		if(isVertical == 1) {
			values = vec2[](vec2(0.0, - pixelSize.y*2), vec2(0.0, - pixelSize.y), vec2(0.0, 0.0), vec2(0.0, pixelSize.y), vec2(0.0, pixelSize.y*2));
		}
		else {
			values = vec2[](vec2(- pixelSize.x*2, 0.0), vec2(- pixelSize.x, 0.0), vec2(0.0, 0.0), vec2(pixelSize.x, 0.0), vec2(pixelSize.x*2, 0.0));
		}

		for (int i = 0; i < 5; i++) {
			vec4 tmp = texture2D(diffuseTex, IN.texCoord.xy + values[i]);
			gl_FragColor += tmp * weights[i];
		}
	}
	else {
		gl_FragColor = texture2D(diffuseTex, IN.texCoord);
	}*/
}

