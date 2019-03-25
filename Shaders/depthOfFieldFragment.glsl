#version 450

uniform sampler2D diffuseTex;
uniform sampler2D depthTex;
uniform vec2 pixelSize;
uniform int isVertical;
uniform float postPasses;
uniform float iteration;
uniform float nearPlane;
uniform float farPlane;

in Vertex {
	vec2 texCoord;
	mat4 inverseProj;
} IN;

out vec4 FragColor;

const float weights[7] = float[](0.07, 0.13, 0.19, 0.22, 0.19, 0.13, 0.07); //Using Sigma = 2 and Kernel = 7 @ http://dev.theomader.com/gaussian-kernel-calculator/

void main(void) {
	vec3 pos = vec3((gl_FragCoord.x * pixelSize.x), (gl_FragCoord.y * pixelSize.y), 0.0);
	pos.z = texture(depthTex, pos.xy).r;

	vec4 clip = IN.inverseProj * vec4(pos * 2.0 - 1.0 , 1.0);
	pos = clip.xyz / clip.w;

	if ((pos.z < 2.0 * farPlane) || (pos.z > nearPlane / 2.0) || ((pos.z < farPlane) && (iteration / postPasses < pos.z/farPlane - 1.0)) ||
           ((pos.z > nearPlane) && (iteration / postPasses < nearPlane/pos.z - 1.0))) {
		vec2 values[7];
		if(isVertical == 1) {
			values = vec2[](vec2(0.0, - pixelSize.y*3), vec2(0.0, - pixelSize.y*2), vec2(0.0, - pixelSize.y), vec2(0.0, 0.0), vec2(0.0, pixelSize.y), vec2(0.0, pixelSize.y*2), vec2(0.0, pixelSize.y*3));
		}
		else {
			values = vec2[](vec2(- pixelSize.x*3, 0.0), vec2(- pixelSize.x*2, 0.0), vec2(- pixelSize.x, 0.0), vec2(0.0, 0.0), vec2(pixelSize.x, 0.0), vec2(pixelSize.x*2, 0.0), vec2(pixelSize.x*3, 0.0));
		}

		for (int i = 0; i < 7; i++) {
			vec4 tmp = texture2D(diffuseTex, IN.texCoord.xy + values[i]);
			FragColor += tmp * weights[i];
		}
	}
	else {
		FragColor = texture2D(diffuseTex, IN.texCoord.xy);
	}
}
