#pragma once
#include "../nclgl/OGLRenderer.h"
#include "../nclgl/MeshMaterial.h"
#include "../nclgl/MeshAnimation.h"
class Camera;
class Shader;
class HeightMap;

class Renderer : public OGLRenderer
{
public:
	bool change;
	Renderer(Window& parent);
	~Renderer(void);
	void RenderScene() override;
	void UpdateScene(float dt) override;
	void StartFlash();

protected:
	void DrawHeightmap();
	void DrawWater();
	void DrawSkybox();
	void DrawFlash();
	void LoadMesh(Mesh* mesh, MeshMaterial* material);
	void DrawMesh(Mesh* mesh, MeshMaterial* material, Vector3 meshPosition, Vector3 meshScale, Vector3 meshRotation);
	
	Mesh* m;
	MeshMaterial* mat;
	Mesh* EVA1;
	MeshMaterial* EVAmat;
	MeshAnimation* anim;
	Shader* Meshshader;	
	Shader* lightShader;
	Shader* reflectShader;
	Shader* skyboxShader;
	Shader* flashShader;
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
	float flashIntensity;      
	bool isFlashing;
	float waterRotate;
	float waterCycle;
	Vector3 meshPosition;
	Vector3 meshScale;
	Vector3 meshRotation;
	Vector3 heightmapSize;
	vector <GLuint> matTextures;

};