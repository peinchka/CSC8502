#include "Renderer.h"
#include <iostream> //
#include <Windows.h> //
#include "../nclgl/Mesh.h"
#include <iomanip> // Set number of decimal points displayed with this
#include <sstream>

using namespace std; //

Renderer::Renderer(Window & parent) : OGLRenderer(parent) {
	camera = new Camera(-150.0f, -30.0f, Vector3(1200.0f, 1250.0f, 160.0f));
	camera1 = new Camera(-60.0f, -60.0f, Vector3(100, 500, 1750.0f));
	camera2 = new Camera(0, -90.0f, Vector3(0, 500, 1000));
	camera3 = new Camera(-30.0f, -90.0f, Vector3(100, 500, 1750.0f));

	count = 0.0f;
	pause = false;
	time = 0.0f;
	scene = 0;
	effect = 5;
	lightsRotation = 0.0f;
	waterRotation = 0.0f;

	heightMap = new HeightMap(TEXTUREDIR"terrain.raw");
	heightMap1 = new HeightMap(TEXTUREDIR"terrain.raw"); 
	heightMap2 = new HeightMap(TEXTUREDIR"terrain5.raw");

	quad = Mesh::GenerateQuad();
	quad1 = Mesh::GenerateQuad();
	quad2 = Mesh::GenerateQuad();
	quad3 = Mesh::GenerateQuad();

	nearplane = -1600;
	farplane = -3000;
	postpasses = 30;
	fov = 45;
	doubleVision = 250;
	dVisionVPIncrease = true;
	toneMapThreshold = 250;
	bloomInfluence = 250;
	bloomPerColour = true;
	textureNum = 0;
	motionBlurFrames = 5;

	redGreenSwap = false;
	redBlueSwap = false;
	greenBlueSwap = false;

	noMinSwap = false;
	noMaxSwap = false;

	HUD = false;

	water = Mesh::GenerateQuad();

	light = new Light(Vector3(0, 0, 0), Vector4(1, 1, 1, 1), 5500.0f);
	light1 = new Light(Vector3(0, 0, 0), Vector4(0, 0.5f, 0.8f, 1), 5500.0f);

	pointLights = new Light[LIGHTNUM * LIGHTNUM];
	for (int x = 0; x < LIGHTNUM; ++x) {
		for (int z = 0; z < LIGHTNUM; ++z) {
			Light &l = pointLights[(x * LIGHTNUM) + z];

			float xPos = (RAW_WIDTH * HEIGHTMAP_X / (LIGHTNUM - 1)) * x;
			float zPos = (RAW_HEIGHT * HEIGHTMAP_Z / (LIGHTNUM - 1)) * z;
			l.SetPosition(Vector3(xPos, 100.0f, zPos));

			float r = (float)(rand() % 129) / 128.0f;
			float g = (float)(rand() % 129) / 128.0f;
			float b = (float)(rand() % 129) / 128.0f;
			l.SetColour(Vector4(r, g, b, 1.0f));

			float radius = (RAW_WIDTH * HEIGHTMAP_X / LIGHTNUM);
			l.SetRadius(radius * 1.2f);
		}
	}

//	emitter1 = new ParticleEmitter();
//	emitter2 = new ParticleEmitter();

	hellData = new MD5FileData(MESHDIR"hellknight.md5mesh");
	hellNode = new MD5Node(*hellData);

	hellData->AddAnim(MESHDIR"idle2.md5anim");
	hellNode->PlayAnim(MESHDIR"idle2.md5anim");

	/*hellData->AddAnim(MESHDIR"walk7.md5anim");
	hellNode->PlayAnim(MESHDIR"walk7.md5anim");*/

	//hellData->AddAnim(MESHDIR"attack2.md5anim");
	//hellNode->PlayAnim(MESHDIR"attack2.md5anim");

#ifdef MD5_USE_HARDWARE_SKINNING
	sceneShader = new Shader(SHADERDIR"HWShadowVertex.glsl", SHADERDIR"HWShadowFragment.glsl");
	//	sceneShader = new Shader(SHADERDIR"shadowscenevert.glsl", SHADERDIR"HWShadowFragment.glsl");
	shadowShader = new Shader(SHADERDIR"HWShadowVert.glsl", SHADERDIR"shadowFrag.glsl");
#else
	sceneShader = new Shader(SHADERDIR"shadowscenevert6.glsl", SHADERDIR"shadowscenefrag6.glsl");
	shadowShader = new Shader(SHADERDIR"shadowVert.glsl", SHADERDIR"shadowFrag.glsl");
#endif

	glossSpecShader = new Shader(SHADERDIR"glossSpecVertex.glsl", SHADERDIR"glossSpecFragment.glsl");
	reflectShader = new Shader(SHADERDIR"PerPixelVertex.glsl", SHADERDIR"reflectFragment.glsl");
	skyboxShader = new Shader(SHADERDIR"skyboxVertex.glsl", SHADERDIR"skyboxFragment.glsl");
	skyboxTimedShader = new Shader(SHADERDIR"skyboxVertex.glsl", SHADERDIR"skyboxTimedFragment.glsl");
	lightShader = new Shader(SHADERDIR"bumpVertex.glsl", SHADERDIR"bumpFragment.glsl");
	mirrorShader = new Shader(SHADERDIR"mirrorVertex.glsl", SHADERDIR"mirrorFragment.glsl");
	framerateShader = new Shader(SHADERDIR"framerateVertex.glsl", SHADERDIR"framerateFragment.glsl");

	sceneShader1 = new Shader(SHADERDIR"TexturedVertex.glsl", SHADERDIR"TexturedFragment1.glsl");
	sobelShader = new Shader(SHADERDIR"ProcessVertex.glsl", SHADERDIR"sobelFragment.glsl");
	aaShader = new Shader(SHADERDIR"ProcessVertex.glsl", SHADERDIR"aaFragment.glsl");
	blurShader = new Shader(SHADERDIR"ProcessVertex.glsl", SHADERDIR"blurFragment.glsl");
	depthOfFieldShader = new Shader(SHADERDIR"depthOfFieldVertex.glsl", SHADERDIR"depthOfFieldFragment.glsl");
	bloomInitShader = new Shader(SHADERDIR"TexturedVertex.glsl", SHADERDIR"bloomInitFragment.glsl");
	bloomShader = new Shader(SHADERDIR"TexturedVertex.glsl", SHADERDIR"bloomFragment.glsl");
	combineShader = new Shader(SHADERDIR"TexturedVertex.glsl", SHADERDIR"combineFragment.glsl");
	carbonCopyShader = new Shader(SHADERDIR"carbonCopyVertex.glsl", SHADERDIR"carbonCopyFragment.glsl");
	toneMapShader = new Shader(SHADERDIR"TexturedVertex.glsl", SHADERDIR"toneMapFragment.glsl");
	doubleVisionShader = new Shader(SHADERDIR"TexturedVertex.glsl", SHADERDIR"doubleVisionFragment.glsl");
	motionBlurShader = new Shader(SHADERDIR"TexturedVertex.glsl", SHADERDIR"motionBlurFragment.glsl");
	deferredShader = new Shader(SHADERDIR"bumpVertex.glsl", SHADERDIR"bufferFragment.glsl");
	pointlightShader = new Shader(SHADERDIR"pointlightvertex.glsl", SHADERDIR"pointlightfragmentGlossSpec.glsl");
	deferredCombineShader = new Shader(SHADERDIR"combinevert.glsl", SHADERDIR"combinefrag.glsl");
	particleShader = new Shader(SHADERDIR"vertex.glsl", SHADERDIR"fragment.glsl", SHADERDIR"geometry.glsl");

	if (!particleShader->LinkProgram() || !deferredCombineShader->LinkProgram() || !pointlightShader->LinkProgram() || !deferredShader->LinkProgram() || !motionBlurShader->LinkProgram() || !bloomShader->LinkProgram() || !bloomInitShader->LinkProgram() || !doubleVisionShader->LinkProgram() || !toneMapShader->LinkProgram() || !glossSpecShader->LinkProgram() || !framerateShader->LinkProgram() || !sceneShader->LinkProgram() || !shadowShader->LinkProgram() || !reflectShader->LinkProgram() || !lightShader->LinkProgram() || !skyboxShader->LinkProgram() || !mirrorShader->LinkProgram()) {
		cout << "Error 1!";
		system("pause");
		return;
	}

	if (!skyboxTimedShader->LinkProgram() || !carbonCopyShader->LinkProgram() || !aaShader->LinkProgram() || !combineShader->LinkProgram() || !depthOfFieldShader->LinkProgram() || !sobelShader->LinkProgram() || !blurShader->LinkProgram() || !sceneShader1->LinkProgram()) {
		cout << "Error 2!";
		system("pause");
		return;
	}

	water->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"bestwater.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	heightMap->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"Barren Reds.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	heightMap->SetBumpMap(SOIL_load_OGL_texture(TEXTUREDIR"Barren RedsDOT3.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	heightMap1->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"stonerough_diffuse.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	heightMap1->SetBumpMap(SOIL_load_OGL_texture(TEXTUREDIR"stonerough_bumpmap.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	heightMap1->SetGlossMap(SOIL_load_OGL_texture(TEXTUREDIR"stonerough_glossmap.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	heightMap1->SetSpecularMap(SOIL_load_OGL_texture(TEXTUREDIR"stonerough_specularmap.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	heightMap2->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"stainedglass.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	cubeMap = SOIL_load_OGL_cubemap(
		TEXTUREDIR"rusted_west.JPG", TEXTUREDIR"rusted_east.JPG",
		TEXTUREDIR"rusted_up.JPG", TEXTUREDIR"rusted_down.JPG",
		TEXTUREDIR"rusted_south.JPG", TEXTUREDIR"rusted_north.JPG",
		SOIL_LOAD_RGB,
		SOIL_CREATE_NEW_ID, 0
	);

	cubeMap1 = SOIL_load_OGL_cubemap(
		TEXTUREDIR"Yokohama/posx.JPG", TEXTUREDIR"Yokohama/negx.JPG",
		TEXTUREDIR"Yokohama/posy.JPG", TEXTUREDIR"Yokohama/negy.JPG",
		TEXTUREDIR"Yokohama/posz.JPG", TEXTUREDIR"Yokohama/negz.JPG",
		SOIL_LOAD_RGB,
		SOIL_CREATE_NEW_ID, 0
	);

	cubeMap2 = SOIL_load_OGL_cubemap(
		TEXTUREDIR"IcyHell/posx.tga", TEXTUREDIR"IcyHell/negx.tga",
		TEXTUREDIR"IcyHell/posy.tga", TEXTUREDIR"IcyHell/negy.tga",
		TEXTUREDIR"IcyHell/posz.tga", TEXTUREDIR"IcyHell/negz.tga",
		SOIL_LOAD_RGB,
		SOIL_CREATE_NEW_ID, 0
	);

	if (!water->GetTexture() || !heightMap->GetTexture() || !heightMap->GetBumpMap() || !cubeMap || !cubeMap1 || !cubeMap2) {
		cout << "Error 2!";
		system("pause");
		return;
	}

	if (!heightMap->GetTexture() || !heightMap->GetBumpMap()) {
		cout << "Error 3!";
		system("pause");
		return;
	}

	if (!heightMap1->GetTexture() || !heightMap1->GetBumpMap() || !heightMap1->GetGlossMap() || !heightMap1->GetSpecularMap()) {
		cout << "Error 4!";
		system("pause");
		return;
	}

	if (!heightMap2->GetTexture()) {
		cout << "Error 5!";
		system("pause");
		return;
	}

	sphere = new OBJMesh();
	if (!sphere->LoadOBJMesh(MESHDIR"ico.obj")) {
		cout << "Fook1!";
		system("pause");
		return;
	}

	SetTextureRepeating(quad->GetTexture(), true);
	SetTextureRepeating(heightMap->GetTexture(), true);
	SetTextureRepeating(heightMap->GetBumpMap(), true);
	SetTextureRepeating(water->GetTexture(), true);

	SetTextureRepeating(heightMap1->GetTexture(), true);
	SetTextureRepeating(heightMap1->GetBumpMap(), true);
	SetTextureRepeating(heightMap1->GetGlossMap(), true);
	SetTextureRepeating(heightMap1->GetSpecularMap(), true);

	SetTextureRepeating(heightMap2->GetTexture(), true);

	glGenTextures(1, &shadowTex);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &shadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTex, 0);
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenTextures(1, &shadowTex1);
	glBindTexture(GL_TEXTURE_2D, shadowTex1);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &shadowFBO1);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO1);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTex1, 0);
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	floor = Mesh::GenerateQuad();
	floor->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"brick.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	floor->SetBumpMap(SOIL_load_OGL_texture(TEXTUREDIR"brickDOT3.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	glEnable(GL_DEPTH_TEST);

	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	basicFont = new Font(SOIL_load_OGL_texture(TEXTUREDIR"tahoma.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT), 16, 16);
	fontTexture = SOIL_load_OGL_texture(TEXTUREDIR"tahoma.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT);

	// Generate our scene depth texture ...
	glGenTextures(1, &bufferDepthTex);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
	//	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_FLOAT, NULL);
	//	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);

	for (int i = 0; i < 2; ++i) {
		glGenTextures(1, &bufferColourTex[i]);
		glBindTexture(GL_TEXTURE_2D, bufferColourTex[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	}

	glGenFramebuffers(1, &bufferFBO); // We 'll render the scene into this
	glGenFramebuffers(1, &processFBO); // And do post-processing in this
	glGenFramebuffers(1, &bufferMultiScreenFBO1); // And render to multiple screens in these...
	glGenFramebuffers(1, &bufferMultiScreenFBO2);
	glGenFramebuffers(1, &bufferMultiScreenFBO3);
	glGenFramebuffers(1, &bufferMultiScreenFBO4);
	glGenFramebuffers(1, &bloomFBO);
	glGenFramebuffers(1, &motionBlurFBO);

	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[0], 0);

	for (int i = 0; i < 4; ++i) {
		glGenTextures(1, &bufferMultiScreenColourTex[i]);
		glBindTexture(GL_TEXTURE_2D, bufferMultiScreenColourTex[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

		glGenTextures(1, &bufferMultiScreenDepthTex[i]);
		glBindTexture(GL_TEXTURE_2D, bufferMultiScreenDepthTex[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, bufferMultiScreenFBO1);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferMultiScreenDepthTex[0], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, bufferMultiScreenDepthTex[0], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferMultiScreenColourTex[0], 0);

	glBindFramebuffer(GL_FRAMEBUFFER, bufferMultiScreenFBO2);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferMultiScreenDepthTex[1], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, bufferMultiScreenDepthTex[1], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferMultiScreenColourTex[1], 0);

	glBindFramebuffer(GL_FRAMEBUFFER, bufferMultiScreenFBO3);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferMultiScreenDepthTex[2], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, bufferMultiScreenDepthTex[2], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferMultiScreenColourTex[2], 0);

	glBindFramebuffer(GL_FRAMEBUFFER, bufferMultiScreenFBO4);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferMultiScreenDepthTex[3], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, bufferMultiScreenDepthTex[3], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferMultiScreenColourTex[3], 0);

	glGenTextures(1, &bloomColourTex);
	glBindTexture(GL_TEXTURE_2D, bloomColourTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glGenFramebuffers(1, &bloomFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, bloomFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bloomColourTex, 0);

	for (int i = 0; i < 5; ++i) {
		glGenTextures(1, &motionBlurColourTex[i]);
		glBindTexture(GL_TEXTURE_2D, motionBlurColourTex[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	}

	glGenFramebuffers(1, &motionBlurFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, motionBlurFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, motionBlurColourTex[0], 0);


	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE || !bufferDepthTex || !bufferColourTex[0] || !bloomColourTex || !motionBlurColourTex[0] ||
		!bufferMultiScreenDepthTex[0] || !bufferMultiScreenDepthTex[1] || !bufferMultiScreenDepthTex[2] || !bufferMultiScreenDepthTex[3] ||
		!bufferMultiScreenColourTex[0] || !bufferMultiScreenColourTex[1] || !bufferMultiScreenColourTex[2] || !bufferMultiScreenColourTex[3]) {
		return;
	}

	glGenFramebuffers(1, &deferredFBO);
	glGenFramebuffers(1, &pointLightFBO);

	GLenum buffers[3];
	buffers[0] = GL_COLOR_ATTACHMENT0;
	buffers[1] = GL_COLOR_ATTACHMENT1;
	buffers[2] = GL_COLOR_ATTACHMENT2;

	// Generate our scene depth texture ...
	GenerateScreenTexture(deferredDepthTex, true);
	GenerateScreenTexture(deferredColourTex);
	GenerateScreenTexture(deferredNormalTex);
	GenerateScreenTexture(deferredTexCoordTex);
	GenerateScreenTexture(lightEmissiveTex);
	GenerateScreenTexture(lightSpecularTex);

	// And now attach them to our FBOs
	glBindFramebuffer(GL_FRAMEBUFFER, deferredFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, deferredColourTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, deferredNormalTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, deferredTexCoordTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glDrawBuffers(3, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lightEmissiveTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, lightSpecularTex, 0);
	glDrawBuffers(2, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		return;
	}


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
	init = true;
}

Renderer::~Renderer(void) {

	delete camera;
	delete camera1;
	delete camera2;
	delete camera3;
	delete light;
	delete light1;
	delete[] pointLights;
	delete hellData;
	delete hellNode;
	delete floor;
	delete heightMap;
	delete heightMap1;
	delete heightMap2;
	delete quad;
	delete quad1;
	delete quad2;
	delete quad3;
	delete water;
	delete reflectShader;
	delete skyboxShader;
	delete lightShader;
	delete sceneShader;
	delete shadowShader;
	delete framerateShader;
	delete basicFont;
	delete glossSpecShader;
	delete sceneShader1;
	delete sobelShader;
	delete blurShader;
	delete depthOfFieldShader;
	delete combineShader;
	delete carbonCopyShader;
	delete toneMapShader;
	delete sphere;
	delete mirrorShader;
	delete skyboxTimedShader;
	delete aaShader;
	delete bloomShader;
	delete bloomInitShader;
	delete doubleVisionShader;
	delete motionBlurShader;
	delete pointlightShader;
	delete deferredShader;
	delete deferredCombineShader;
	delete particleShader;

//	delete emitter1;
//	delete emitter2;

	glDeleteTextures(2, bufferColourTex);
	glDeleteTextures(1, &bufferDepthTex);
	glDeleteTextures(1, &bloomColourTex);
	glDeleteTextures(4, bufferMultiScreenColourTex); 
	glDeleteTextures(4, bufferMultiScreenDepthTex);
	glDeleteTextures(5, motionBlurColourTex);

	glDeleteTextures(1, &deferredColourTex);
	glDeleteTextures(1, &deferredNormalTex);
	glDeleteTextures(1, &deferredDepthTex);
	glDeleteTextures(1, &deferredTexCoordTex);
	glDeleteTextures(1, &lightEmissiveTex);
	glDeleteTextures(1, &lightSpecularTex);
	glDeleteTextures(1, &shadowTex);
	
	glDeleteFramebuffers(1, &shadowFBO);
	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &processFBO);
	glDeleteFramebuffers(1, &bloomFBO);
	glDeleteFramebuffers(1, &motionBlurFBO);
	glDeleteFramebuffers(1, &deferredFBO);
	glDeleteFramebuffers(1, &pointLightFBO);
	glDeleteFramebuffers(1, &bufferMultiScreenFBO1);
	glDeleteFramebuffers(1, &bufferMultiScreenFBO2);
	glDeleteFramebuffers(1, &bufferMultiScreenFBO3);
	glDeleteFramebuffers(1, &bufferMultiScreenFBO4);
	currentShader = NULL;
}

void Renderer::GenerateScreenTexture(GLuint& into, bool depth) {
	glGenTextures(1, &into);
	glBindTexture(GL_TEXTURE_2D, into);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, depth ? GL_DEPTH24_STENCIL8 : GL_RGBA8, width, height, 0, depth ? GL_DEPTH_STENCIL : GL_RGBA, depth ? GL_UNSIGNED_INT_24_8 : GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::SetShaderLight(const Light& l) {
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "lightPos"), 1, (float*)&light->GetPosition());
	glUniform4fv(glGetUniformLocation(currentShader->GetProgram(), "lightColour"), 1, (float*)&l.GetColour());
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "lightRadius"), l.GetRadius());
}

void Renderer::SetShaderLight1(const Light& l) {
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "lightPos1"), 1, (float*)&light1->GetPosition());
	glUniform4fv(glGetUniformLocation(currentShader->GetProgram(), "lightColour1"), 1, (float*)&l.GetColour());
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "lightRadius1"), l.GetRadius());
}

void Renderer::SetShaderLightDeferred(const Light& l) {
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "lightPos"), 1, (float*)&l.GetPosition());
	glUniform4fv(glGetUniformLocation(currentShader->GetProgram(), "lightColour"), 1, (float*)&l.GetColour());
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "lightRadius"), l.GetRadius());
}

void Renderer::UpdateScene(float msec) {
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_RIGHT)) {
		scene += 1;
		scene = scene % 4;
		if (scene == 1) {
			lightposition = Vector3((RAW_HEIGHT * HEIGHTMAP_X / 2.0f), 100.0f, (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f));
			lightposition1 = Vector3((RAW_HEIGHT * HEIGHTMAP_X / 2.0f), 50.0f, (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f));
		}
		if (scene == 2) {
			camera = new Camera(-150.0f, -30.0f, Vector3(1200.0f, 1250.0f, 160.0f));
		}
		time = 0.0f;
	}
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_LEFT)) {
		scene -= 1;
		scene = max(scene, 0);
		scene = scene % 4;
		if (scene == 1) {
			lightposition = Vector3((RAW_HEIGHT * HEIGHTMAP_X / 2.0f), 100.0f, (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f));
			lightposition1 = Vector3((RAW_HEIGHT * HEIGHTMAP_X / 2.0f), 50.0f, (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f));
		}
		if (scene == 2) {
			camera = new Camera(-150.0f, -30.0f, Vector3(1200.0f, 1250.0f, 160.0f));
		}
		time = 0.0f;
	}
	if (!pause) {
		camera->UpdateCamera(msec);
		camera1->UpdateCamera(msec);
		camera2->UpdateCamera(msec);
		camera3->UpdateCamera(msec);
		UpdateLights(msec);
//		emitter1->Update(msec);
//		emitter2->Update(msec);
		viewMatrix = camera->BuildViewMatrix();
		count = count + msec;
		if (count > 200.0f) {
			count = count - 200.0f;
			framerate = 1000.0f / msec;
		}
		lightsRotation = msec * 0.01f;
		waterRotation += msec / 1000.0f;
		hellNode->Update(msec);
		time = time + msec / 1000.0f;
	}
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_P)) {
		pause = !pause;
	}
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_PERIOD)) {
		HUD = !HUD;
	}
}

void Renderer::RenderScene() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);

	if (scene == 0) {
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
		DrawSkybox();
		DrawHeightmap();
		DrawWater();
		DrawMirror();
		if (HUD) {
			DrawFramerate();
		}
		if (camera->GetPosition().y < 1850 && abs((RAW_WIDTH * HEIGHTMAP_X / 2.0f) + 200.0f - camera->GetPosition().x) < 50.0f && abs((RAW_HEIGHT * HEIGHTMAP_Z / 2.0f) + 400.0f - camera->GetPosition().z) < 50.0f)
		{
			lightposition = Vector3((RAW_HEIGHT * HEIGHTMAP_X / 2.0f), 100.0f, (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f));
			lightposition1 = Vector3((RAW_HEIGHT * HEIGHTMAP_X / 2.0f), 50.0f, (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f));
			scene = 1;
		}
	}
	if (scene == 1) {
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap2);
		DrawTimedSkybox();
		DrawShadowScene();
		DrawCombinedScene();
		if (HUD) {
			DrawFramerate();
		}
	}
	if (scene == 2) {
		FillBuffers();
		DrawPointLights();
		DrawDeferredSkybox();
		CombineBuffers();
		if (HUD) {
			DrawFramerate();
		}
	}
	if (scene == 3) {
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap1);

		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_Y)) {
			effect = 1;
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_U)) {
			effect = 2;
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_I)) {
			effect = 3;
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_O)) {
			effect = 4;
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_L)) {
			effect = 5;  //No effect
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_K)) {
			effect = 6;
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_J)) {
			effect = 7;
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_H)) {
			effect = 8;
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_T)) {
			effect = 9;
		}

		if (effect == 1) {
			DrawFinalScene();
			DrawDepthOfField();
			PresentScene();
		}
		if (effect == 2) {
			DrawFinalScene();
			DrawSobel();
			PresentScene();
		}
		if (effect == 3) {
			DrawFinalScene();
			DrawAntiAliasing(true);
			DrawAntiAliasing(false);
			DrawAntiAliasing(true);
			DrawAntiAliasing(false);
			PresentScene();
		}
		if (effect == 4) {
			DrawFinalScene();
			DrawToneMap();
		}
		if (effect == 5) {
			DrawFinalScene();
			PresentScene();
		}
		if (effect == 6) {
			DrawMultiScreenScene();
			PresentMultiScreenScene();
		}
		if (effect == 7) {
			DrawFinalScene();
			DrawDoubleVision();
		}
		if (effect == 8) {
			DrawFinalScene();
			DrawBloom();
		}
		if (effect == 9) {
			DrawFinalScene();
			if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_1)) {
				motionBlurFrames--;
				if (motionBlurFrames == 0) motionBlurFrames = 1;
			}
			if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_2)) {
				motionBlurFrames++;
				if (motionBlurFrames == 6) motionBlurFrames = 5;
			}
			DrawMotionBlur(textureNum);
			textureNum++;
			textureNum = textureNum % motionBlurFrames;
			PresentMotionBlurScene();
		}
		if (effect == 10) {
//			DrawParticles();
		}	
	}

	SwapBuffers();
}

void Renderer::UpdateLights(float msec) {
	lightSpeed = 10.0f;
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_R))
		lightposition = Vector3((RAW_HEIGHT * HEIGHTMAP_X / 2.0f), 100.0f, (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f));
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_K))
		lightposition.y -= lightSpeed;
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_I))
		lightposition.y += lightSpeed;
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_J))
		lightposition.x -= lightSpeed;
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_L))
		lightposition.x += lightSpeed;
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_Y))
		lightposition.z -= lightSpeed;
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_H))
		lightposition.z += lightSpeed;

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_R))
		lightposition1 = Vector3((RAW_HEIGHT * HEIGHTMAP_X / 2.0f), 50.0f, (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f));
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_DOWN))
		lightposition1.y -= lightSpeed;
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_UP))
		lightposition1.y += lightSpeed;
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_N))
		lightposition1.x -= lightSpeed;
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_M))
		lightposition1.x += lightSpeed;
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_Z))
		lightposition1.z -= lightSpeed;
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_X))
		lightposition1.z += lightSpeed;

	light->SetPosition(lightposition);
	light1->SetPosition(lightposition1);
}

void Renderer::DrawShadowScene() {
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	SetCurrentShader(shadowShader);

	modelMatrix.ToIdentity();
	viewMatrix = Matrix4::BuildViewMatrix(light->GetPosition(), Vector3(4000, 100, 4000));
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
	UpdateShaderMatrices();

	if (light->GetPosition().y > 0.0f) {
		heightMap1->Draw();
		DrawShadowFloor();
		DrawShadowMesh();
		lightExists = 1;
	}
	else {
		lightExists = 0;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO1);
	glClear(GL_DEPTH_BUFFER_BIT);
	modelMatrix.ToIdentity();
	viewMatrix = Matrix4::BuildViewMatrix(light1->GetPosition(), Vector3(0, 0, 0));
	UpdateShaderMatrices();

	if (light1->GetPosition().y > 0.0f) {
		heightMap1->Draw();
		DrawShadowFloor();
		DrawShadowMesh();
		lightExists1 = 1;
	}
	else {
		lightExists1 = 0;
	}
	glUseProgram(0);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glDisable(GL_CULL_FACE);
}

void Renderer::DrawCombinedScene() {
	SetCurrentShader(glossSpecShader);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "bumpTex"), 1);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "glossTex"), 2);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "specularTex"), 3);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "shadowTex"), 4);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "shadowTex1"), 5);
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "drawFloor"), 1); //Only for hardware skinning!!!!
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "lightExists"), lightExists);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "lightExists1"), lightExists1);

	SetShaderLight(*light);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, heightMap1->GetTexture());
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, heightMap1->GetBumpMap());
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, heightMap1->GetGlossMap());
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, heightMap1->GetSpecularMap());
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, shadowTex);

	SetShaderLight1(*light1);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, shadowTex1);

	viewMatrix = Matrix4::BuildViewMatrix(light->GetPosition(), Vector3(4000, 100, 4000));
	shadowMatrix = biasMatrix * (projMatrix * viewMatrix);

	viewMatrix1 = Matrix4::BuildViewMatrix(light1->GetPosition(), Vector3(0, 0, 0));
	shadowMatrix1 = biasMatrix * (projMatrix * viewMatrix1);

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "shadowMatrix"), 1, false, (float*)&shadowMatrix);
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "shadowMatrix1"), 1, false, (float*)&shadowMatrix1);

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_V))
		viewMatrix = Matrix4::Rotation(-camera->GetPitch(), Vector3(1, 0, 0)) * Matrix4::Rotation(-camera->GetYaw(), Vector3(0, 1, 0)) * Matrix4::Rotation(-camera->GetRoll(), Vector3(0, 0, 1)) * Matrix4::Translation(-light->GetPosition());
	else if (Window::GetKeyboard()->KeyDown(KEYBOARD_B))
		viewMatrix = Matrix4::Rotation(-camera->GetPitch(), Vector3(1, 0, 0)) * Matrix4::Rotation(-camera->GetYaw(), Vector3(0, 1, 0)) * Matrix4::Rotation(-camera->GetRoll(), Vector3(0, 0, 1)) * Matrix4::Translation(-light1->GetPosition());
	else
		viewMatrix = camera->BuildViewMatrix();

	modelMatrix.ToIdentity();
	UpdateShaderMatrices();
	heightMap1->Draw();
	DrawFloor();

	SetCurrentShader(sceneShader);

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "lightExists"), lightExists);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "lightExists1"), lightExists1);

	SetShaderLight(*light);
	SetShaderLight1(*light1);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "bumpTex"), 1);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "shadowTex"), 4);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "shadowTex1"), 5);
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "drawFloor"), 1); //Only for hardware skinning!!!!
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "shadowMatrix"), 1, false, (float*)&shadowMatrix);
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "shadowMatrix1"), 1, false, (float*)&shadowMatrix1);

	DrawMesh();

	glUseProgram(0);
}

void Renderer::DrawFloor() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, width, height);
	modelMatrix.ToIdentity();
	modelMatrix = Matrix4::Translation(Vector3(2100.0f, 50, -500.0f)) * Matrix4::Rotation(90, Vector3(1, 0, 0)) * Matrix4::Scale(Vector3(2000, 450, 1));
	Matrix4 tempMatrix = shadowMatrix * modelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, (float*)&modelMatrix);
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "viewMatrix"), 1, false, (float*)&viewMatrix);
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "projMatrix"), 1, false, (float*)&projMatrix);
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "shadowMatrix"), 1, false, (float*)&tempMatrix);

	viewMatrix1 = Matrix4::BuildViewMatrix(light1->GetPosition(), Vector3(0, 0, 0));
	Matrix4 shadowMatrix1 = biasMatrix * projMatrix * viewMatrix1 * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "shadowMatrix1"), 1, false, (float*)&shadowMatrix1);

	floor->Draw();
}

void Renderer::DrawMesh() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, width, height);

	for (int x = 1; x < RAW_WIDTH; x+=40) {
		for (int z = 1; z < RAW_HEIGHT; z+=40) {
			int offset = (x * RAW_HEIGHT) + z;
			modelMatrix.ToIdentity();
			modelMatrix = Matrix4::Translation(Vector3(x * HEIGHTMAP_X, heightMap1->GetVertices()[offset].y, z * HEIGHTMAP_Z)) * Matrix4::Rotation(x * HEIGHTMAP_X * z * HEIGHTMAP_Z, Vector3(0, 1, 0));
			Matrix4 tempMatrix = textureMatrix * modelMatrix;
			textureMatrix.ToIdentity();
			glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, (float*)&modelMatrix);
			glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "viewMatrix"), 1, false, (float*)&viewMatrix);
			glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "projMatrix"), 1, false, (float*)&projMatrix);
			glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "textureMatrix"), 1, false, (float*)&textureMatrix);
			glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "shadowMatrix"), 1, false, (float*)&tempMatrix);

			viewMatrix1 = Matrix4::BuildViewMatrix(light1->GetPosition(), Vector3(0, 0, 0));
			Matrix4 textureMatrix1 = biasMatrix * projMatrix * viewMatrix1 * modelMatrix;
			glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "shadowMatrix1"), 1, false, (float*)&textureMatrix1);

			hellNode->Draw(*this);
		}
	}
}

void Renderer::DrawShadowFloor() {
	modelMatrix.ToIdentity();
	modelMatrix = Matrix4::Translation(Vector3(2100.0f, 50, -500.0f)) * Matrix4::Rotation(90, Vector3(1, 0, 0)) * Matrix4::Scale(Vector3(2000, 450, 1));
	UpdateShaderMatrices();
	floor->Draw();
}

void Renderer::DrawShadowMesh() {
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	for (int x = 1; x < RAW_WIDTH; x += 40) {
		for (int z = 1; z < RAW_HEIGHT; z += 40) {
			int offset = (x * RAW_HEIGHT) + z;
			modelMatrix.ToIdentity();
			modelMatrix = Matrix4::Translation(Vector3(x * HEIGHTMAP_X, heightMap1->GetVertices()[offset].y, z * HEIGHTMAP_Z)) * Matrix4::Rotation(x * HEIGHTMAP_X * z * HEIGHTMAP_Z, Vector3(0, 1, 0));
			UpdateShaderMatrices();
			hellNode->Draw(*this);
		}
	}
	glDisable(GL_CULL_FACE);
}

void Renderer::DrawSkybox() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, width, height);
	glDepthMask(GL_FALSE);
	SetCurrentShader(skyboxShader);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "cubeTex"), 3);
	UpdateShaderMatrices();
	quad->Draw();
	glUseProgram(0);
	glDepthMask(GL_TRUE);
}

void Renderer::DrawTimedSkybox() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, width, height);
	glDepthMask(GL_FALSE);
	SetCurrentShader(skyboxTimedShader);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "cubeTex"), 3);
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "time"), time);
	UpdateShaderMatrices();
	quad->Draw();
	glUseProgram(0);
	glDepthMask(GL_TRUE);
}

void Renderer::DrawDeferredSkybox() {
	glBindFramebuffer(GL_FRAMEBUFFER, deferredFBO);
	glViewport(0, 0, width, height);
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, deferredNormalTex, 0);

	std::vector<GLubyte> emptyData(width * height * 4, 0);
	glBindTexture(GL_TEXTURE_2D, deferredNormalTex);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_BGRA, GL_UNSIGNED_BYTE, &emptyData[0]);
	glDepthMask(GL_FALSE);

	SetCurrentShader(skyboxTimedShader);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "cubeTex"), 3);
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "time"), time);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap2);

	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_EQUAL, 0, ~0);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	UpdateShaderMatrices();
	quad->Draw();

	glDisable(GL_STENCIL_TEST);

	glUseProgram(0);
	glDepthMask(GL_TRUE);
}

void Renderer::FillBuffers() {
	glBindFramebuffer(GL_FRAMEBUFFER, deferredFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glClear(GL_STENCIL_BUFFER_BIT);

	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 2, ~0);
	glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);

	SetCurrentShader(sceneShader);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, heightMap1->GetTexture());
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, heightMap1->GetBumpMap());
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "bumpTex"), 1);
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
	modelMatrix.ToIdentity();
	UpdateShaderMatrices();
	heightMap1->Draw();
	glUseProgram(0);

	glDisable(GL_STENCIL_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawPointLights() {
	SetCurrentShader(pointlightShader);
	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	glBlendFunc(GL_ONE, GL_ONE);

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "depthTex"), 3);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "normTex"), 4);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "texCoordTex"), 5);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "glossTex"), 6);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "specTex"), 7);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, deferredDepthTex);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, deferredNormalTex);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, deferredTexCoordTex);

	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, heightMap1->GetGlossMap());

	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, heightMap1->GetSpecularMap());

	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

	glUniform2f(glGetUniformLocation(currentShader->GetProgram(), "pixelSize"), 1.0f / width, 1.0f / height);

	Vector3 translate = Vector3((RAW_HEIGHT * HEIGHTMAP_X / 2.0f), 500, (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f));

	Matrix4 pushMatrix = Matrix4::Translation(-translate);
	Matrix4 popMatrix = Matrix4::Translation(translate);

	for (int x = 0; x < LIGHTNUM; ++x) {
		for (int z = 0; z < LIGHTNUM; ++z) {
			Light &l = pointLights[(x * LIGHTNUM) + z];
			float radius = l.GetRadius();

			modelMatrix = popMatrix * Matrix4::Rotation(lightsRotation, Vector3(0, 1, 0)) * pushMatrix * Matrix4::Translation(l.GetPosition()) * Matrix4::Scale(Vector3(radius, radius, radius));

			l.SetPosition(modelMatrix.GetPositionVector());

			SetShaderLightDeferred(l);

			UpdateShaderMatrices();

			float dist = (l.GetPosition() - camera->GetPosition()).Length();
			if (dist < radius) {
				glCullFace(GL_FRONT);
			}
			else {
				glCullFace(GL_BACK);
			}
			sphere->Draw();
		}
	}

	glCullFace(GL_BACK);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.2f, 0.2f, 0.2f, 1);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);
}

void Renderer::CombineBuffers() {

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	SetCurrentShader(deferredCombineShader);

	modelMatrix.ToIdentity();
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	UpdateShaderMatrices();

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 2);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "emissiveTex"), 3);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "specularTex"), 4);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "skyBoxTex"), 5);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, deferredColourTex);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, lightEmissiveTex);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, lightSpecularTex);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, deferredNormalTex);

	quad->Draw();

	glUseProgram(0);
}


void Renderer::DrawHeightmap() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, width, height);
	SetCurrentShader(lightShader);
	SetShaderLight(*light);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, heightMap->GetTexture());
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, heightMap->GetBumpMap());
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "bumpTex"), 1);

	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "time"), time);

	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();

	UpdateShaderMatrices();

	heightMap->Draw();
	glUseProgram(0);
}

void Renderer::DrawWater() {
	SetCurrentShader(reflectShader);
	SetShaderLight(*light);
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)& camera->GetPosition());

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "cubeTex"), 2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, water->GetTexture());
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
	float heightX = (RAW_WIDTH * HEIGHTMAP_X / 2.0f);

	float heightY = 128 * HEIGHTMAP_Y / 3.0f + 10.0f;

	float heightZ = (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f);

	modelMatrix = Matrix4::Translation(Vector3(heightX, heightY, heightZ)) * Matrix4::Scale(Vector3(heightX, 1, heightZ)) * Matrix4::Rotation(90, Vector3(1.0f, 0.0f, 0.0f));

	textureMatrix = Matrix4::Scale(Vector3(10.0f, 10.0f, 10.0f)) * Matrix4::Rotation(waterRotation, Vector3(0.0f, 0.0f, 1.0f));
	
	viewMatrix = camera->BuildViewMatrix();
	
	UpdateShaderMatrices();

	water->Draw();

	glUseProgram(0);
}

void Renderer::DrawMirror() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, width, height);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	SetCurrentShader(mirrorShader);
	SetShaderLight(*light);
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)& camera->GetPosition());
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "cubeTex"), 3);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap1);

	float heightX = (RAW_WIDTH * HEIGHTMAP_X / 2.0f) + 200.0f;

	float heightY = 128 * HEIGHTMAP_Y / 3.0f + 800.0f;

	float heightZ = (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f) + 400.0f;

	modelMatrix = Matrix4::Translation(Vector3(heightX, heightY, heightZ)) * Matrix4::Scale(Vector3(100.0f, 1000.0f, 100.0f)) * Matrix4::Rotation(waterRotation*10.0f, Vector3(0.0f, 1.0f, 0.0f));

	UpdateShaderMatrices();

	quad1->Draw();
	glDisable(GL_BLEND);
	glUseProgram(0);
}

void Renderer::DrawFramerate() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, width, height);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	SetCurrentShader(framerateShader);
	glUseProgram(currentShader->GetProgram());
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "framerateTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fontTexture);

	stringstream ss;
	ss << fixed << setprecision(1) << framerate;
	string str = ss.str();

	DrawText("Framerate: " + str + "fps", Vector3(0, 0, 0), 16.0f);

	stringstream ssx;
	ssx << fixed << setprecision(1) << camera->GetPosition().x;
	string strx = ssx.str();

	stringstream ssy;
	ssy << fixed << setprecision(1) << camera->GetPosition().y;
	string stry = ssy.str();

	stringstream ssz;
	ssz << fixed << setprecision(1) << camera->GetPosition().z;
	string strz = ssz.str();

	DrawText("Camera: x=" + strx + ", y=" + stry + ", z=" + strz, Vector3(0, 20, 0), 16.0f);
	glUseProgram(0);
	glDisable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Renderer::DrawText(const std::string &text, const Vector3 &position, const float size, const bool perspective) {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	TextMesh* mesh = new TextMesh(text, *basicFont);

	textureMatrix.ToIdentity();
	if (perspective) {
		modelMatrix = Matrix4::Translation(position) * Matrix4::Scale(Vector3(size, size, 1));
		viewMatrix = camera->BuildViewMatrix();
		projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
	}
	else {
		modelMatrix = Matrix4::Translation(Vector3(position.x, height - position.y, position.z)) * Matrix4::Scale(Vector3(size, size, 1));
		viewMatrix.ToIdentity();
		projMatrix = Matrix4::Orthographic(-1.0f, 1.0f, (float)width, 0.0f, (float)height, 0.0f);
	}
	UpdateShaderMatrices();
	mesh->Draw();
	delete mesh;
}

void Renderer::DrawFinalScene() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if (effect == 7 && dVisionVPIncrease) { 
		glViewport(0, 0, width + doubleVision, height + doubleVision);
	}
	else {
		glViewport(0, 0, width, height);
	}
	glEnable(GL_DEPTH_TEST);
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[0], 0);
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glDepthMask(GL_FALSE);
	SetCurrentShader(skyboxShader);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "cubeTex"), 3);
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "time"), time);
	UpdateShaderMatrices();
	quad->Draw();
	glUseProgram(0);
	glDepthMask(GL_TRUE);

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_9)) {
		fov = min(90, fov--);
		fov = max(1, fov);
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_0)) {
		fov = min(90, fov++);
		fov = max(1, fov);
	}

	SetCurrentShader(sceneShader1);
	modelMatrix.ToIdentity();
	projMatrix = Matrix4::Perspective(1, 10000, (float)width / (float)height, float(fov));
	viewMatrix = camera->BuildViewMatrix();
	UpdateShaderMatrices();
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, heightMap2->GetTexture());
	heightMap2->Draw();

	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	SetCurrentShader(depthOfFieldShader);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "depthTex"), 1);
	glUseProgram(0);
}

void Renderer::DrawMultiScreenScene() {
	glBindFramebuffer(GL_FRAMEBUFFER, bufferMultiScreenFBO1);
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glViewport(0, 0, width, height);

	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
	viewMatrix = camera->BuildViewMatrix();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferMultiScreenColourTex[0], 0);

	glDepthMask(GL_FALSE);
	SetCurrentShader(skyboxShader);
	UpdateShaderMatrices();
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "cubeTex"), 3);
	quad->Draw();
	glDepthMask(GL_TRUE);

	SetCurrentShader(sceneShader1);
	UpdateShaderMatrices();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, heightMap2->GetTexture());
	
	heightMap2->Draw();


	glBindFramebuffer(GL_FRAMEBUFFER, bufferMultiScreenFBO2);
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	viewMatrix = camera1->BuildViewMatrix();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferMultiScreenColourTex[1], 0);

	glDepthMask(GL_FALSE);
	SetCurrentShader(skyboxShader);
	UpdateShaderMatrices();
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "cubeTex"), 3);
	quad->Draw();
	glDepthMask(GL_TRUE);

	SetCurrentShader(sceneShader1);
	UpdateShaderMatrices();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, heightMap2->GetTexture());

	heightMap2->Draw();


	glBindFramebuffer(GL_FRAMEBUFFER, bufferMultiScreenFBO3);
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	viewMatrix = camera2->BuildViewMatrix();
	UpdateShaderMatrices();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferMultiScreenColourTex[2], 0);

	glDepthMask(GL_FALSE);
	SetCurrentShader(skyboxShader);
	UpdateShaderMatrices();
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "cubeTex"), 3);
	quad->Draw();
	glDepthMask(GL_TRUE);

	SetCurrentShader(sceneShader1);
	UpdateShaderMatrices();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, heightMap2->GetTexture());

	heightMap2->Draw();


	glBindFramebuffer(GL_FRAMEBUFFER, bufferMultiScreenFBO4);
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	viewMatrix = camera3->BuildViewMatrix();
	UpdateShaderMatrices();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferMultiScreenColourTex[3], 0);

	glDepthMask(GL_FALSE);
	SetCurrentShader(skyboxShader);
	UpdateShaderMatrices();
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "cubeTex"), 3);
	quad->Draw();
	glDepthMask(GL_TRUE);

	SetCurrentShader(sceneShader1);
	UpdateShaderMatrices();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, heightMap2->GetTexture());

	heightMap2->Draw();

	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawBlur() {
	glBindFramebuffer(GL_FRAMEBUFFER, processFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1], 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	SetCurrentShader(blurShader);
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	viewMatrix.ToIdentity();
	UpdateShaderMatrices();

	glDisable(GL_DEPTH_TEST);
	glUniform2f(glGetUniformLocation(currentShader->GetProgram(), "pixelSize"), 1.0f / width, 1.0f / height);
	for (int i = 0; i < postpasses; ++i) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1], 0);
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "isVertical"), 0);

		quad->SetTexture(bufferColourTex[0]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, bufferColourTex[0]);

		quad->Draw();

		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "isVertical"), 1);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[0], 0);

		quad->SetTexture(bufferColourTex[1]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, bufferColourTex[1]);
		quad->Draw();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);

	glEnable(GL_DEPTH_TEST);
}

void Renderer::DrawDepthOfField() {
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_1)) {
		postpasses = max(2, postpasses--);
		postpasses = min(100, postpasses);
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_2)) {
		postpasses = max(1, postpasses++);
		postpasses = min(50, postpasses);
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_6)) {
		farplane = min(-1800, farplane -= 10);
		farplane = max(-10000, farplane);
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_5)) {
		farplane = min(-1800, farplane += 10);
		farplane = max(-10000, farplane);
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_4)) {
		nearplane = min(-100, nearplane -= 10);
		nearplane = max(-1800, nearplane);
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_3)) {
		nearplane = min(-100, nearplane += 10);
		nearplane = max(-1800, nearplane);
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_R)) {
		nearplane = -1600;
		farplane = -3000;
		fov = 45;
	}

	SetCurrentShader(depthOfFieldShader);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "depthTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);
	quad->SetBumpMap(bufferDepthTex);

	glBindFramebuffer(GL_FRAMEBUFFER, processFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1], 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	viewMatrix.ToIdentity();
	UpdateShaderMatrices();

	Matrix4 originalProjMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "originalProjMatrix"), 1, false, (float*)&originalProjMatrix);
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "postPasses"), float(postpasses));

	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "nearPlane"), (float)nearplane);
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "farPlane"), (float)farplane);

	glDisable(GL_DEPTH_TEST);
	glUniform2f(glGetUniformLocation(currentShader->GetProgram(), "pixelSize"), 1.0f / width, 1.0f / height);
	for (int i = 0; i < postpasses; ++i) {
		glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "iteration"), float(i));
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1], 0);
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "isVertical"), 0);

		quad->SetTexture(bufferColourTex[0]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, bufferColourTex[0]);

		quad->Draw();

		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "isVertical"), 1);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[0], 0);

		quad->SetTexture(bufferColourTex[1]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, bufferColourTex[1]);
		quad->Draw();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);

	glEnable(GL_DEPTH_TEST);
}

void Renderer::DrawSobel() {
	glBindFramebuffer(GL_FRAMEBUFFER, processFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1], 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	SetCurrentShader(sobelShader);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 2);
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	viewMatrix.ToIdentity();
	UpdateShaderMatrices();

	glDisable(GL_DEPTH_TEST);
	glUniform2f(glGetUniformLocation(currentShader->GetProgram(), "pixelSize"), 1.0f / width, 1.0f / height);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex[0]);

	quad->Draw();

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[0], 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	SetCurrentShader(carbonCopyShader);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 2);
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	viewMatrix.ToIdentity();
	UpdateShaderMatrices();

	glDisable(GL_DEPTH_TEST);
	glUniform2f(glGetUniformLocation(currentShader->GetProgram(), "pixelSize"), 1.0f / width, 1.0f / height);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex[1]);

	quad->Draw();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);
	glEnable(GL_DEPTH_TEST);
}

void Renderer::DrawAntiAliasing(bool vert) {
	glBindFramebuffer(GL_FRAMEBUFFER, processFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1], 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	SetCurrentShader(aaShader);
	if (vert)
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "isVertical"), 1);
	else
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "isVertical"), 0);

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 2);
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	viewMatrix.ToIdentity();
	UpdateShaderMatrices();

	glDisable(GL_DEPTH_TEST);
	glUniform2f(glGetUniformLocation(currentShader->GetProgram(), "pixelSize"), 1.0f / width, 1.0f / height);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex[0]);

	quad->Draw();

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[0], 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	SetCurrentShader(carbonCopyShader);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 2);
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	viewMatrix.ToIdentity();
	UpdateShaderMatrices();

	glDisable(GL_DEPTH_TEST);
	glUniform2f(glGetUniformLocation(currentShader->GetProgram(), "pixelSize"), 1.0f / width, 1.0f / height);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex[1]);

	quad->Draw();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);
	glEnable(GL_DEPTH_TEST);
}

void Renderer::DrawBloom() {
	glDisable(GL_DEPTH_TEST);

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_3)) {
		bloomInfluence = min(300, bloomInfluence++);
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_4)) {	
		bloomInfluence = max(200, bloomInfluence--);
	}
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_5)) {
		bloomPerColour = !bloomPerColour;
	}
	SetCurrentShader(bloomInitShader);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "bloomInfluence"), bloomInfluence);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "bloomPerColour"), bloomPerColour);
	modelMatrix.ToIdentity();
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	viewMatrix.ToIdentity();
	UpdateShaderMatrices();
	glBindFramebuffer(GL_FRAMEBUFFER, bloomFBO);
	glClear(GL_COLOR_BUFFER_BIT);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bloomColourTex, 0);
	quad->SetTexture(bufferColourTex[0]);
	quad->Draw();

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_1)) {
		postpasses = max(0, postpasses--);
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_2)) {
		postpasses = min(100, postpasses++);
	}

	SetCurrentShader(bloomShader);
	modelMatrix.ToIdentity();
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	viewMatrix.ToIdentity();
	UpdateShaderMatrices();
	glUniform2f(glGetUniformLocation(currentShader->GetProgram(), "pixelSize"), 1.0f / width, 1.0f / height);
	for (int i = 0; i < postpasses; ++i) {
		glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1], 0);
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "isVertical"), 0);
		quad->SetTexture(bloomColourTex);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, bloomColourTex);
		quad->Draw();

		glBindFramebuffer(GL_FRAMEBUFFER, bloomFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bloomColourTex, 0);
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "isVertical"), 1);
		quad->SetTexture(bufferColourTex[1]);
		quad->Draw();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	SetCurrentShader(combineShader);
	modelMatrix.ToIdentity();
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	viewMatrix.ToIdentity();
	UpdateShaderMatrices();
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 2);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex1"), 3);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex[0]);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, bloomColourTex);
	glViewport(0, 0, width, height);
	quad->Draw();

	glUseProgram(0);
	glEnable(GL_DEPTH_TEST);
}

void Renderer::PresentMultiScreenScene() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	SetCurrentShader(sceneShader1);

	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	viewMatrix.ToIdentity();
	UpdateShaderMatrices();

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferMultiScreenColourTex[0]);
	glViewport(0, height / 2, width / 2, height / 2);
	quad->SetTexture(bufferMultiScreenColourTex[0]);
	quad->Draw();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferMultiScreenColourTex[1]);
	glViewport(width / 2, height / 2, width / 2, height / 2);
	quad1->SetTexture(bufferMultiScreenColourTex[1]);
	quad1->Draw();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferMultiScreenColourTex[2]);
	glViewport(0, 0, width / 2, height / 2);
	quad2->SetTexture(bufferMultiScreenColourTex[2]);
	quad2->Draw();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferMultiScreenColourTex[3]);
	glViewport(width / 2, 0, width / 2, height / 2);
	quad3->SetTexture(bufferMultiScreenColourTex[3]);
	quad3->Draw();

//	DrawFramerate();

	glUseProgram(0);
}

void Renderer::DrawToneMap() {
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_2)) {
		toneMapThreshold = max(0, toneMapThreshold--);
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_1)) {
		toneMapThreshold = min(500, toneMapThreshold++);
	}
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_3)) {
		redGreenSwap = !redGreenSwap;
	}
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_4)) {
		redBlueSwap = !redBlueSwap;
	}
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_5)) {
		greenBlueSwap = !greenBlueSwap;
	}
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_6)) {
		noMinSwap = !noMinSwap;
	}
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_7)) {
		noMaxSwap = !noMaxSwap;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	SetCurrentShader(toneMapShader);

	modelMatrix.ToIdentity();
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	viewMatrix.ToIdentity();
	UpdateShaderMatrices();
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 2);
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "toneMap"), (float)toneMapThreshold);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "redGreenSwap"), redGreenSwap);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "redBlueSwap"), redBlueSwap);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "greenBlueSwap"), greenBlueSwap);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "noMinSwap"), noMinSwap);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "noMaxSwap"), noMaxSwap);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex[0]);
	glViewport(0, 0, width, height);
	quad->Draw();

	glUseProgram(0);
}

void Renderer::DrawDoubleVision() {
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_1)) {
		doubleVision = max(0, doubleVision-=2);
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_2)) {
		doubleVision = min(500, doubleVision+=2);
	}
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_3)) {
		dVisionVPIncrease = !dVisionVPIncrease;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, width, height);

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	SetCurrentShader(doubleVisionShader);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "doubleVision"), doubleVision);

	modelMatrix.ToIdentity();
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	viewMatrix.ToIdentity();
	UpdateShaderMatrices();
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 2);
	if (dVisionVPIncrease) {
		glUniform2f(glGetUniformLocation(currentShader->GetProgram(), "pixelSize"), 1.0f / (width + doubleVision), 1.0f / (height + doubleVision));
	}
	else {
		glUniform2f(glGetUniformLocation(currentShader->GetProgram(), "pixelSize"), 1.0f / width, 1.0f / height);
	}
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex[0]);
	quad->Draw();

	glUseProgram(0);
}

void Renderer::DrawMotionBlur(int texture) {
	glDisable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, motionBlurFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, motionBlurColourTex[texture], 0);

	SetCurrentShader(sceneShader1);
	modelMatrix.ToIdentity();
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	viewMatrix.ToIdentity();
	UpdateShaderMatrices();
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex[0]);
	glViewport(0, 0, width, height);
	quad->Draw();

	glUseProgram(0);
	glEnable(GL_DEPTH_TEST);
}

void Renderer::PresentMotionBlurScene() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	SetCurrentShader(motionBlurShader);
	modelMatrix.ToIdentity();
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	viewMatrix.ToIdentity();
	UpdateShaderMatrices();
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 2);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex1"), 3);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex2"), 4);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex3"), 5);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex4"), 6);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "motionBlurFrames"), motionBlurFrames);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, motionBlurColourTex[0]);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, motionBlurColourTex[1]);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, motionBlurColourTex[2]);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, motionBlurColourTex[3]);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, motionBlurColourTex[4]);
	glViewport(0, 0, width, height);
	quad->Draw();

	glUseProgram(0);
}

void Renderer::PresentScene() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	SetCurrentShader(sceneShader1);

	modelMatrix.ToIdentity();
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	viewMatrix.ToIdentity();
	UpdateShaderMatrices();
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex[0]);
	glViewport(0, 0, width, height);
	quad->Draw();

	glUseProgram(0);
}

void Renderer::DrawParticles() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	SetCurrentShader(particleShader);
	SetShaderParticleSize(emitter1->GetParticleSize());
	emitter1->SetParticleSize(8.0f);
	emitter1->SetParticleVariance(1.0f);
	emitter1->SetLaunchParticles(16);
	emitter1->SetParticleLifetime(2000.0f);
	emitter1->SetParticleSpeed(0.1f);
	UpdateShaderMatrices();

//	emitter1->Draw();
//	emitter2->Draw();

	glUseProgram(0);
}

void Renderer::SetShaderParticleSize(float f) {
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "particleSize"), f);
}

void Renderer::TimeLine1(float time) {
	if (time <= 10.0) {
		//Do nothing until fully loaded!
	}
	if ((time > 10.0) && (time <= 15.0)) {
		cameraposition = Vector3((RAW_HEIGHT * HEIGHTMAP_X / 2.0f), 500.0f, (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f)) + Vector3(80.0f, 0.0f, 10.0f) * time;
		camera->SetPosition(cameraposition);
	}
	if ((time > 15.0) && (time <= 20.0)) {
		cameraposition = Vector3((RAW_HEIGHT * HEIGHTMAP_X / 2.0f), 500.0f, (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f)) + Vector3(0, -10, 0) * time;
		camera->SetPosition(cameraposition);
		camera->SetPitch(5 * sin(6.28f * time));
	}
	if ((time > 20.0) && (time <= 30.0)) {
		camera->SetYaw(36.0f * time);
	}
	if (time > 30.0) {
		time = 0.0f;
		scene = 2;
	}
}

void Renderer::TimeLine2(float time) {

}

void Renderer::TimeLine3(float time) {

}

void Renderer::TimeLine4(float time) {
	if (time <= 10.0) {
		DrawFinalScene();
		DrawDepthOfField();
		PresentScene();

		if (time <= 3.0) {
			postpasses = 2 + (int)time * 30;
			nearplane = min(-100, nearplane -= 10);
		}
		if ((time > 3.0) && (time <= 9.0)) {
			postpasses -= (int)time * 30;
			nearplane = max(400, nearplane += 10);
			farplane = max(-1800, farplane += 10);
		}
		if ((time > 9.0) && (time <= 10.0)) {
			postpasses -= (int)time * 30;
			farplane = min(-1800, farplane -= 10);
		}
	}
	if ((time > 10.0) && (time <= 15.0)) {
		DrawFinalScene();
		DrawSobel();
		PresentScene();
	}
	if ((time > 15.0) && (time <= 20.0)) {
		DrawFinalScene();
		DrawAntiAliasing(true);
		DrawAntiAliasing(false);
		PresentScene();
	}
	if (time > 20.0) {
		time = 0.0f;
		scene = 0;
	}
}