#version 450 core

uniform sampler2D diffuseTex;
uniform sampler2D bumpTex;
uniform sampler2DShadow shadowTex;
uniform sampler2DShadow shadowTex1;

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

	float shadow = 1.0;
	float shadow1 = 1.0;


	if(IN.shadowProj.w > 0.0) { // New !
		shadow = textureProj(shadowTex, IN.shadowProj);
	}

	lambert *= shadow; // New !

	if(IN.shadowProj1.w > 0.0) { // New !
		shadow1 = textureProj(shadowTex1, IN.shadowProj1);
	}

	lambert1 *= shadow1; // New !

	vec3 colour = (diffuse.rgb * lightColour.rgb);
	vec3 colour1 = (diffuse.rgb * lightColour1.rgb);
	colour += (lightColour.rgb * sFactor) * 0.33;
	colour1 += (lightColour1.rgb * sFactor1) * 0.33;
	FragColor = vec4(colour * lambert + colour1 * lambert1, diffuse.a);
	FragColor.rgb += diffuse.rgb * (lightColour.rgb + lightColour1.rgb) * 0.2;

//	FragColor.rgb = vec3(shadow, shadow1 ,0);


//+ colour1 * 
//FragColor = vec4(lambert ,lambert ,lambert ,1);
//FragColor.xyz = normal;
}