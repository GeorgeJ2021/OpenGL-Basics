#pragma once
#include "../nclgl/OGLRenderer.h"
#include "../nclgl/MeshMaterial.h"
#include "../nclgl/MeshAnimation.h"
#include "../nclgl/SceneNode.h"
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
	void DrawNode(SceneNode* node);
	void LoadMesh(Mesh* mesh, MeshMaterial* material, std::vector<GLuint>& matTextures);
	void DrawMesh(Mesh* mesh, MeshMaterial* material, Vector3 meshPosition, Vector3 meshScale, Vector3 meshRotation);
	void DrawShinji();

	Mesh* m;
	MeshMaterial* mat;
	Mesh* EVA1;
	MeshMaterial* EVAmat;
	MeshAnimation* anim;
	Mesh* Shinji;
	MeshMaterial* Shinjimat;
	MeshAnimation* Shinjianim;
	SceneNode* root;
	SceneNode* evaNode;
	SceneNode* evaNode2;
	SceneNode* RockNode;
	SceneNode* ShinNode;
	Shader* Meshshader;	
	Shader* lightShader;
	Shader* reflectShader;
	Shader* skyboxShader;
	Shader* flashShader;
	Shader* Anmshader;
	HeightMap* heightMap;
	Mesh* quad;
	Camera* camera;
	GLuint cubeMap;
	GLuint waterTex;
	GLuint earthTex;
	GLuint earthBump;
	GLuint waterBump;
	GLuint daySkybox,nightSkybox;
	GLuint dayWaterTex, nightWaterTex;
	GLuint EVATex;
	GLuint RockTex;
	Light* dayLight;
	Light* nightLight;
	float flashIntensity;      
	bool isFlashing;
	float waterRotate;
	float waterCycle;
	int currentFrame;
	float frameTime;
	Vector3 meshPosition;
	Vector3 meshScale;
	Vector3 meshRotation;
	Vector3 heightmapSize;
	vector <GLuint> matTextures;
	vector <GLuint> matTextures2;
	vector < GLuint > ShinjiTextures;
};