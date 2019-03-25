#version 450

uniform sampler2D diffuseTex;
//uniform float blend;

in Vertex {
	vec3 position;
	vec2 texCoord;
} IN;

out vec4 gl_FragColor;

void main(void){
	if(IN.position.y/256.0 < 0.95) {
		gl_FragColor =  ((IN.position.y /256.0)) /* vec4(0.3,1,0.5,1) */ * texture(diffuseTex, IN.texCoord);// * vec4(0.01*(IN.position.x), 0.01*(IN.position.z), (IN.position.x) * (IN.position.y), 1); 
	}
	else
	{
		gl_FragColor = ((IN.position.y/256.0)-0.95) * 7.0 * vec4(1,1,0.8,1);// * texture(diffuseTex, IN.texCoord);
	}
}