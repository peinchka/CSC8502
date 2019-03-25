#pragma once

#include "../nclgl/OGLRenderer.h"
#include "../nclgl/Camera.h"
#include "../nclgl/MD5Mesh.h"
#include "../nclgl/MD5Node.h"
#include "../nclgl/HeightMap.h"
#include "../nclgl/Light.h"
#include "../nclgl/TextMesh.h"
#include "../nclgl/OBJmesh.h"
#include "../nclgl/ParticleEmitter.h"

#define SHADOWSIZE 2048
#define LIGHTNUM 10

class Renderer : public OGLRenderer {
public:
	Renderer(Window & parent);
	virtual ~Renderer(void);
	virtual void UpdateScene(float msec);
	virtual void RenderScene();


protected:
	void Renderer::UpdateLights(float msec);
	void SetLightPosition(Vector3 position) { lightposition = position; }
	void SetLightPosition1(Vector3 position1) { lightposition1 = position1; }
	void GenerateScreenTexture(GLuint& into, bool = false);
	Vector3 GetLightPosition() const { return lightposition; }
	Vector3 GetLightPosition1() const { return lightposition1; }
	void DrawText(const std::string &text, const Vector3 &position, const float size = 10.0f, const bool perspective = false);
	void SetShaderParticleSize(float f);
	ParticleEmitter*	emitter1;
	ParticleEmitter*	emitter2;

	void DrawHeightmap();
	void DrawWater();
	void DrawMirror();
	void DrawSkybox();
	void DrawTimedSkybox();
	void DrawMesh();
	void DrawFloor();
	void DrawShadowMesh();
	void DrawShadowFloor();
	void DrawShadowScene();
	void DrawCombinedScene();
	void DrawFramerate();
	void DrawToneMap();
	void SetShaderLight(const Light& l);
	void SetShaderLight1(const Light& l);
	void SetShaderLightDeferred(const Light& l);
	void TimeLine1(float time);
	void TimeLine2(float time);
	void TimeLine3(float time);
	void TimeLine4(float time);
	void PresentScene();
	void PresentMultiScreenScene();
	void DrawBlur();
	void DrawDepthOfField();
	void DrawBloom();
	void DrawSobel();
	void DrawAntiAliasing(bool vert);
	void DrawFinalScene();
	void DrawMultiScreenScene();
	void DrawDoubleVision();
	void DrawMotionBlur(int texture);
	void PresentMotionBlurScene();
	void DrawDeferredSkybox();
	void FillBuffers();
	void DrawPointLights();
	void CombineBuffers();
	void DrawParticles();

	Vector3 lightposition;
	Vector3 lightposition1;
	Vector3 cameraposition;
	Matrix4 viewMatrix1;
	Matrix4 shadowMatrix;
	Matrix4 shadowMatrix1;

	Shader* sceneShader;
	Shader* shadowShader;
	Shader* lightShader;
	Shader* reflectShader;
	Shader* skyboxShader;
	Shader* mirrorShader;
	Shader* framerateShader;
	Shader* glossSpecShader;
	Shader* skyboxTimedShader;
	Shader* sceneShader1;
	Shader* sobelShader;
	Shader* aaShader;
	Shader* blurShader;
	Shader* depthOfFieldShader;
	Shader* toneMapShader;
	Shader* carbonCopyShader;
	Shader* bloomShader;
	Shader* bloomInitShader;
	Shader* combineShader;
	Shader* doubleVisionShader;
	Shader* motionBlurShader;
	Shader* pointlightShader;
	Shader* deferredShader;
	Shader* deferredCombineShader;
	Shader* particleShader;

	GLuint shadowTex;
	GLuint shadowTex1;
	GLuint shadowFBO;
	GLuint shadowFBO1;

	GLuint cubeMap;
	GLuint cubeMap1;
	GLuint cubeMap2;
	GLuint fontTexture;

	Font*	basicFont;

	HeightMap* heightMap;
	HeightMap* heightMap1;
	HeightMap* heightMap2;

	MD5FileData* hellData;
	MD5Node* hellNode;
	Mesh* floor;

	Camera* camera;
	Camera* camera1;
	Camera* camera2;
	Camera* camera3;

	Light* light;
	Light* light1;
	Light* pointLights;

	Mesh* water;

	Mesh* quad;
	Mesh* quad1;
	Mesh* quad2;
	Mesh* quad3;

	OBJMesh* sphere;

	GLuint bufferFBO;
	GLuint processFBO;
	GLuint motionBlurFBO;

	GLuint bloomFBO;
	GLuint bloomColourTex;

	GLuint deferredFBO;
	GLuint pointLightFBO;
	
	GLuint bufferMultiScreenFBO1;
	GLuint bufferMultiScreenFBO2;
	GLuint bufferMultiScreenFBO3;
	GLuint bufferMultiScreenFBO4;
	GLuint bufferColourTex[2];
	GLuint bufferDepthTex;
	GLuint bufferMultiScreenColourTex[4];
	GLuint bufferMultiScreenDepthTex[4];
	GLuint motionBlurColourTex[5];

	GLuint deferredDepthTex;
	GLuint deferredColourTex;
	GLuint deferredNormalTex;
	GLuint deferredTexCoordTex;
	GLuint lightEmissiveTex;
	GLuint lightSpecularTex;

	int nearplane, farplane;
	int postpasses;
	int fov;
	int scene;
	int effect;
	int toneMapThreshold;
	int doubleVision;
	int bloomInfluence;
	int textureNum;
	int motionBlurFrames;

	float waterRotation;
	float lightsRotation;
	float count;
	float framerate;
	float time;
	float lightSpeed;

	bool pause;
	bool redGreenSwap;	
	bool redBlueSwap;	
	bool greenBlueSwap;
	bool noMinSwap;
	bool noMaxSwap;
	bool dVisionVPIncrease;
	bool HUD;
	bool bloomPerColour;

	int lightExists;
	int lightExists1;
};