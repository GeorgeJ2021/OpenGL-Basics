#pragma once
#include "../nclgl/OGLRenderer.h"

class Camera;
class Shader;
class HeightMap;

class Renderer : public OGLRenderer
{
public:
	bool change;
	Renderer(Window& parent);
	~Renderer(void);
	void ToggleScene();
	void RenderScene() override;
	void UpdateScene(float dt) override;

protected:
	void DrawHeightmap();
	void DrawWater();
	void DrawSkybox();
	void LoadScene1();
	void LoadScene2();
	
	Shader* lightShader;
	Shader* reflectShader;
	Shader* skyboxShader;
	HeightMap* heightMap;
	Mesh* quad;
	Light* light;
	Camera* camera;
	GLuint cubeMap;
	GLuint waterTex;
	GLuint earthTex;
	GLuint earthBump;
	GLuint waterBump;
	GLuint daySkybox,nightSkybox;
	GLuint dayWaterTex, nightWaterTex;
	Light* dayLight;
	Light* nightLight;
	float waterRotate;
	float waterCycle;

};