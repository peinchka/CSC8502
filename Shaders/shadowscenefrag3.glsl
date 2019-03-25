#version 450 core

uniform sampler2D diffuseTex;
uniform sampler2D bumpTex;
//uniform sampler2DShadow shadowTex[2];
uniform sampler2DShadow shadowTex;
uniform sampler2DShadow shadowTex1;
uniform sampler2DShadow shadowTex2;
uniform sampler2DShadow shadowTex3;
uniform sampler2DShadow shadowTex4;
uniform sampler2DShadow shadowTex5;
uniform sampler2DShadow shadowTex10;
//uniform sampler2D shadowTex;
//uniform sampler2D shadowTex10;
uniform vec3 cameraPos;
uniform vec4 lightColour;
uniform vec3 lightPos;
uniform float lightRadius;
uniform vec4 lightColour1;
uniform vec3 lightPos1;
uniform float lightRadius1;

in Vertex {
	vec3 colour;
 	vec2 texCoord;
 	vec3 normal;
 	vec3 tangent;
	vec3 binormal;
 	vec3 worldPos;
 	vec4 shadowProj;
 	vec4 shadowProj1;
	vec4 shadowProj2;
	vec4 shadowProj3;
	vec4 shadowProj4;
	vec4 shadowProj5;
	vec4 shadowProj10;
} IN;

out vec4 FragColor;

void main(void) {
 	vec4 diffuse = texture2D(diffuseTex, IN.texCoord);
 	mat3 TBN = mat3(IN.tangent, IN.binormal, IN.normal);
 	vec3 normal = normalize(TBN * (texture2D(bumpTex, IN.texCoord).rgb * 2.0 - vec3(1.0)));
 	vec3 incident = normalize(lightPos - IN.worldPos);
 	float lambert = max(0.0, dot(incident, normal));
 	float dist = length(lightPos - IN.worldPos);
 	float atten = 1.0 - clamp(dist/lightRadius, 0.0, 1.0);
 	vec3 viewDir = normalize(cameraPos - IN.worldPos);
 	vec3 halfDir = normalize(incident + viewDir);
 	float rFactor = max(0.0, dot(halfDir, normal));
 	float sFactor = pow(rFactor, 33.0);

 	vec3 incident1 = normalize(lightPos1 - IN.worldPos);
 	float lambert1 = max(0.0, dot(incident1, normal));
 	float dist1 = length(lightPos1 - IN.worldPos);
 	float atten1 = 1.0 - clamp(dist1/lightRadius1, 0.0, 1.0);
 	vec3 halfDir1 = normalize(incident1 + viewDir);
	float rFactor1 = max(0.0, dot(halfDir1, normal));
 	float sFactor1 = pow(rFactor1, 33.0);
 	float shadow = 1.0f;
 	float shadow1 = 1.0f;

/*	vec3 shadowNDC = IN.shadowProj.xyz / IN.shadowProj.w;
	if(abs(shadowNDC.x) < 1.0f && abs(shadowNDC.y) < 1.0f && abs(shadowNDC.z) < 1.0f) {
		vec2 shadowVec = vec2(shadowNDC.x * 0.5f + 0.5f, shadowNDC.y * 0.5f + 0.5f);
		float shadowZ = (texture(shadowTex, shadowVec)).x;
		float bias = 0.0f;
		if(shadowNDC.z + bias > shadowZ) {
			shadow = 0.0f;
		}
	}

	vec3 shadowNDC1 = IN.shadowProj1.xyz / IN.shadowProj1.w;
	if(abs(shadowNDC1.x) < 1.0f && abs(shadowNDC1.y) < 1.0f && abs(shadowNDC1.z) < 1.0f) {
		vec2 shadowVec1 = vec2(shadowNDC1.x * 0.5f + 0.5f, shadowNDC1.y * 0.5f + 0.5f);
		float shadowZ1 = texture(shadowTex10, shadowVec1).x;
		float bias1 = 0.0f;
		if(shadowNDC1.z + bias1 > shadowZ1) {
			shadow1 = 0.0f;
		}
	}
*/

	if(IN.shadowProj.w > 0.0) {
  		shadow = textureProj(shadowTex, IN.shadowProj);
 	}

/*	if(IN.shadowProj1.w > 0.0) {
  		shadow = textureProj(shadowTex1, IN.shadowProj1);
 	}

	if(IN.shadowProj2.w > 0.0) {
  		shadow = textureProj(shadowTex2, IN.shadowProj2);
 	}

	if(IN.shadowProj3.w > 0.0) {
  		shadow = textureProj(shadowTex3, IN.shadowProj3);
 	}

	if(IN.shadowProj4.w > 0.0) {
  		shadow = textureProj(shadowTex4, IN.shadowProj4);
 	}

	if(IN.shadowProj5.w > 0.0) {
  		shadow = textureProj(shadowTex5, IN.shadowProj5);
 	}
	
 	if(IN.shadowProj10.w > 0.0) {
  		shadow1 = textureProj(shadowTex10, IN.shadowProj10);
 	}
*/
	lambert *= shadow;
	lambert1 *= shadow1;

 	vec3 colour = (diffuse.rgb * lightColour.rgb);
 	vec3 colour1 = (diffuse.rgb * lightColour1.rgb);
 	colour += (lightColour.rgb * sFactor) * 0.33;
 	colour1 += (lightColour1.rgb * sFactor1) * 0.33;
 	FragColor = vec4(colour * atten * lambert + colour1 * atten1 * lambert1, diffuse.a);
 	FragColor.rgb += diffuse.rgb * (lightColour.rgb + lightColour1.rgb) * 0.2;

//FragColor.rgb = vec3(1, shadow, shadow1);
//+ colour1 * 
//FragColor = vec4(lambert ,lambert ,lambert ,1);
//FragColor.xyz = normal;
}
