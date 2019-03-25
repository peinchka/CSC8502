#version 150
uniform sampler2D diffuseTex;
uniform sampler2D diffuseTex1;
uniform float blend;

in Vertex {
	vec4 colour;
	vec2 texCoord;
} IN;

out vec4 gl_FragColor;

void main(void){
	gl_FragColor = blend * texture(diffuseTex1, IN.texCoord) + (1 - blend) * vec4((texture(diffuseTex, IN.texCoord)).rgb, 0.5);
//	gl_FragColor = blend * IN.colour + (1 - blend) * texture(diffuseTex, IN.texCoord);
//	gl_FragColor = IN.colour * (blend * texture(diffuseTex1, IN.texCoord) + (1 - blend) * vec4((texture(diffuseTex, IN.texCoord)).rgb, 0.5));
}