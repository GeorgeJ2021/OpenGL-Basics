#include "Renderer.h"
#include "../nclgl/Light.h"
#include "../nclgl/Heightmap.h"
#include "../nclgl/Shader.h"
#include "../nclgl/Camera.h"

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
	quad = Mesh::GenerateQuad();
	heightMap = new HeightMap(TEXTUREDIR "noise4.png");
	earthTex = SOIL_load_OGL_texture(TEXTUREDIR "sand.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	earthBump = SOIL_load_OGL_texture(TEXTUREDIR "sandnormal.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	waterBump = SOIL_load_OGL_texture(TEXTUREDIR "waterbump.PNG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	daySkybox = SOIL_load_OGL_cubemap(TEXTUREDIR "dayleft.png", TEXTUREDIR "dayright.png", TEXTUREDIR "daytop.png", TEXTUREDIR "daybottom.png",
		TEXTUREDIR "dayback.png", TEXTUREDIR "dayfront.png", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

	nightSkybox = SOIL_load_OGL_cubemap(TEXTUREDIR "cubemap_4.png", TEXTUREDIR "cubemap_5.png", TEXTUREDIR "cubemap_up.png", TEXTUREDIR "cubemap_down.png",
		TEXTUREDIR "cubemap_1.png", TEXTUREDIR "cubemap_0.png", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

	dayWaterTex = SOIL_load_OGL_texture(TEXTUREDIR "water.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	nightWaterTex = SOIL_load_OGL_texture(TEXTUREDIR "redfanta.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	if (!earthTex || !earthBump || !daySkybox || !nightSkybox || !dayWaterTex || !nightWaterTex) {
		return;
	}
	SetTextureRepeating(earthTex, true);
	SetTextureRepeating(earthBump, true);
	SetTextureRepeating(waterBump, true);
	SetTextureRepeating(dayWaterTex, true);
	SetTextureRepeating(nightWaterTex, true);
	reflectShader = new Shader("reflectVertex.glsl", "reflectFragment.glsl");
	skyboxShader = new Shader("skyboxVertex.glsl", "skyboxFragment.glsl");
	lightShader = new Shader("PerPixelVertex.glsl", "PerPixelFragment.glsl");
	flashShader = new Shader("fadeVertex.glsl", "fadeFragment.glsl");
	Meshshader = new Shader("TexturedVertex.glsl", "texturedFragment.glsl");
	Anmshader = new Shader("SkinningVertex.glsl", "texturedFragment.glsl");
	rainShader = new Shader("rainVertex.glsl ", "rainFragment.glsl");
	if (!reflectShader->LoadSuccess() || !skyboxShader->LoadSuccess() || !lightShader->LoadSuccess() || !Meshshader->LoadSuccess()) {
		return;
	}
	Vector3 map = Vector3(0.5f, 1.0f, 0.4f);
	heightmapSize = heightMap->GetHeightmapSize();
	camera = new Camera(0.0f, -180.0f, heightmapSize * map);
	camera->settrack(map, heightmapSize);

	m = Mesh::LoadFromMeshFile("Rock1.msh");
	mat = new MeshMaterial("Rock1.mat");
	RockTex = SOIL_load_OGL_texture(TEXTUREDIR "rocks_gray_small_Albedo.TGA", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	EVA1 = Mesh::LoadFromMeshFile("EVAmass.msh");
	EVAmat = new MeshMaterial("EVAmass.mat");
	EVATex = SOIL_load_OGL_texture(TEXTUREDIR "e26_201.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS); 
	//anim = new MeshAnimation("EVA01.anm");

	Shinji = Mesh::LoadFromMeshFile("shinji1.msh");
	Shinjimat = new MeshMaterial("shinji1.mat");
	Shinjianim = new MeshAnimation("shinji1.anm");

	// Load textures from material file
	LoadMesh(m, mat, matTextures);
	LoadMesh(EVA1, EVAmat,matTextures2);
	LoadMesh(Shinji, Shinjimat, ShinjiTextures);

	root = new SceneNode();

	// Create the EVA SceneNode
	Matrix4 rotation = Matrix4::Rotation(180, Vector3(0, 1, 0));

	evaNode = new SceneNode(EVA1, Vector4(1, 1, 1, 1));  // Using white color for now
	evaNode->SetModelScale(Vector3(200.0f, 200.0f, 200.0f));    // Adjust scale as needed
	evaNode->SetTransform(Matrix4::Translation(heightmapSize * Vector3(0.4f, 0.15f, 0.8f)));  // Adjust position
	evaNode->SetTexture(EVATex); // Set the first submesh texture (if there’s more, we can handle that in rendering)
	evaNode->SetRotation(rotation);
	// Add EVA node to the root of the scene graph
	root->AddChild(evaNode);
	
	evaNode2 = new SceneNode(EVA1, Vector4(1, 1, 1, 1));  // Using white color for now
	evaNode2->SetModelScale(Vector3(200.0f, 200.0f, 200.0f));    // Adjust scale as needed
	evaNode2->SetTransform(Matrix4::Translation(heightmapSize * Vector3(0.8f, 0.15f, 0.8f)));  // Adjust position
	evaNode2->SetTexture(EVATex); // Set the first submesh texture (if there’s more, we can handle that in rendering)
	evaNode2->SetRotation(rotation);
	root->AddChild(evaNode2);

	RockNode = new SceneNode(m, Vector4(1, 1, 1, 1));
	RockNode->SetModelScale(Vector3(100.0f, 100.0f, 100.0f));    // Adjust scale as needed
	RockNode->SetTransform(Matrix4::Translation(heightmapSize * Vector3(0.0f, 0.47f, 0.52f)));  // Adjust position
	RockNode->SetTexture(RockTex); // Set the first submesh texture (if there’s more, we can handle that in rendering)
	//RockNode->SetRotation(rotation);
	root->AddChild(RockNode);


	ShinNode = new SceneNode(Shinji, Vector4(1, 1, 1, 1));
	ShinNode->SetModelScale(Vector3(100.0f, 100.0f, 100.0f));    // Adjust scale as needed
	ShinNode->SetTransform(Matrix4::Translation(heightmapSize * Vector3(0.5f, 0.5f, 0.5f)));  // Adjust position
	//ShinNode->SetTexture(RockTex); // Set the first submesh texture (if there’s more, we can handle that in rendering)
	////RockNode->SetRotation(rotation);
	root->AddChild(ShinNode);

	// Set mesh position on the heightmap
	/*meshPosition = heightmapSize * Vector3(0.5f, 0.28f, 0.8f);
	Vector3 meshScale = Vector3(100.0f, 100.0f, 100.0f);
	Vector3 meshRotation = Vector3(0, 1, 0);*/
	//light = new Light(heightmapSize * Vector3(1.5f, 4.0f, 0.5f), Vector4(0.8, 0.6, 1, 1), heightmapSize.x * 2);

	dayLight = new Light(heightmapSize * Vector3(0.5f, 10.0f, 0.5f), Vector4(1.0, 1.0, 1.0, 1), heightmapSize.x * 10.0f);
	nightLight = new Light(heightmapSize * Vector3(1.0f, 4.0f, 0.5f), Vector4(0.5, 0.6, 0.8, 1), heightmapSize.x * 2.0f);

	triangle = Mesh::GenerateQuad();
	rainSpeed = 200.0f;

	for (int i = 0; i < 1000; ++i) {
		rainPositions.push_back(Vector3(
			static_cast<float>(rand()) / RAND_MAX * heightmapSize.x,
			static_cast<float>(rand()) / RAND_MAX * 600.0f + 600.0f,
			static_cast<float>(rand()) / RAND_MAX * heightmapSize.z
		));
	}

	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	waterRotate = 0.0f;
	waterCycle = 0.0f;
	flashIntensity = 0.0f;
	currentFrame = 0;
	frameTime = 0.0f;
	isFlashing = false;
	init = true;

}

Renderer ::~Renderer(void) {
	delete camera;
	delete heightMap;
	delete quad;

	glDeleteTextures(1, &daySkybox);
	glDeleteTextures(1, &nightSkybox);
	glDeleteTextures(1, &dayWaterTex);
	glDeleteTextures(1, &nightWaterTex);

	delete flashShader;
	delete dayLight;
	delete nightLight;
	delete reflectShader;
	delete skyboxShader;
	delete lightShader;
	delete Anmshader;
	//delete light;
	delete m;
	delete mat;
	delete EVA1;
	delete EVAmat;
	delete Shinji;
	delete Shinjimat;
	delete Meshshader;

}

void Renderer::UpdateScene(float dt) {
	camera->UpdateCamera(dt);
	viewMatrix = camera->BuildViewMatrix();
	waterRotate += dt * 2.0f;
	waterCycle += dt * 0.25f;
	frameTime -= dt;
	while (frameTime < 0.0f) {
		currentFrame = (currentFrame + 1) % Shinjianim->GetFrameCount();
		frameTime += 1.0f / Shinjianim->GetFrameRate();
	}

	for (auto& pos : rainPositions) {
		pos.y -= rainSpeed * dt;


		if (pos.y < 0.0f) {
			pos.y = static_cast<float>(rand()) / RAND_MAX * 600.0f + 600.0f;
			pos.x = static_cast<float>(rand()) / RAND_MAX * heightmapSize.x;
			pos.z = static_cast<float>(rand()) / RAND_MAX * heightmapSize.z;
		}
	}

	root->Update(dt);
	if (isFlashing) {
		flashIntensity += dt * 2.0f;   // Adjust the multiplier for flash speed
		if (flashIntensity >= 1.0f) {
			flashIntensity = 1.0f;
			isFlashing = false; 
			change = !change;
		}
	}
	else if (flashIntensity > 0.0f) {
		flashIntensity = std::max(flashIntensity - dt * 2.0f, 0.0f); 
	}

}

void Renderer::RenderScene() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	DrawSkybox();
	DrawHeightmap();
	DrawWater();
	DrawNode(root);
	//DrawShinji();
	DrawFlash();
	if (change == false)
	{
		DrawRain();
	}
	
	
}

void Renderer::DrawSkybox() {
	glDepthMask(GL_FALSE);
	BindShader(skyboxShader);
	UpdateShaderMatrices();
	GLuint currentSkybox = change ? daySkybox : nightSkybox;
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, currentSkybox);
	quad->Draw();
	glDepthMask(GL_TRUE);
}

void Renderer::LoadMesh(Mesh* mesh, MeshMaterial* material, std::vector<GLuint>& matTextures)
{
	for (int i = 0; i < mesh->GetSubMeshCount(); ++i) {
		const MeshMaterialEntry* matEntry = material->GetMaterialForLayer(i);
		const string* filename = nullptr;
		matEntry->GetEntry("Diffuse", &filename);
		string path = TEXTUREDIR + *filename;
		GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
		matTextures.emplace_back(texID);
	}

}



void Renderer::DrawHeightmap() {
	BindShader(lightShader);
	Light* currentLight = change ? dayLight : nightLight;
	SetShaderLight(*currentLight);
	//SetShaderLight(*light);
	glUniform3fv(glGetUniformLocation(lightShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());
	glUniform1i(glGetUniformLocation(lightShader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, earthTex);
	glUniform1i(glGetUniformLocation(lightShader->GetProgram(), "bumpTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, earthBump);
	glUniform4f(glGetUniformLocation(lightShader->GetProgram(), "ambientLightColor"), 0.7f, 0.6f, 1.0f, 1.0f);

	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();
	UpdateShaderMatrices();
	heightMap->Draw();
}

void Renderer::DrawWater() {
	BindShader(reflectShader);
	glUniform3fv(glGetUniformLocation(reflectShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());
	glUniform1i(glGetUniformLocation(reflectShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(reflectShader->GetProgram(), "cubeTex"), 2); 

	GLuint currentWaterTex = change ? dayWaterTex : nightWaterTex;
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, currentWaterTex);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, change ? daySkybox : nightSkybox);

	glUniform1i(glGetUniformLocation(reflectShader->GetProgram(), "bumpTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, waterBump); 

	Vector3 hSize = heightMap->GetHeightmapSize();
	modelMatrix = Matrix4::Translation(hSize * 0.5f) * Matrix4::Scale(hSize * 0.5f) * Matrix4::Rotation(90, Vector3(1, 0, 0));
	textureMatrix = Matrix4::Translation(Vector3(waterCycle, 0.0f, waterCycle)) * Matrix4::Scale(Vector3(10, 10, 10)) * Matrix4::Rotation(waterRotate, Vector3(0, 0, 1));
	UpdateShaderMatrices();
	quad->Draw();
}

void Renderer::DrawFlash() {
	if (flashIntensity <= 0.0f) return;  
	glDisable(GL_DEPTH_TEST);  

	BindShader(flashShader);
	glUniform1f(glGetUniformLocation(flashShader->GetProgram(), "flashIntensity"), flashIntensity);

	quad->Draw();  

	glEnable(GL_DEPTH_TEST);
}

void Renderer::StartFlash() {
	flashIntensity = 0.0f;  
	isFlashing = true;
}

void Renderer::DrawNode(SceneNode* node) {
	if (!node) return;
	BindShader(lightShader);
	// Set the model matrix for each node

	modelMatrix = node->GetWorldTransform() * node->GetRotation() * Matrix4::Scale(node->GetModelScale());
	UpdateShaderMatrices();

	glUniform1i(glGetUniformLocation(Meshshader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(lightShader->GetProgram(), "diffuseTex"), 0);
	glUniform4f(glGetUniformLocation(lightShader->GetProgram(), "ambientLightColor"), 0.7f, 0.6f, 1.0f, 1.0f);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, node->GetTexture());

	if (change == false && (node == evaNode || node == evaNode2))
	{
		for (int i = 0; i < EVA1->GetSubMeshCount(); ++i) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, matTextures2[i]);
			EVA1->DrawSubMesh(i);
		}
	}

	else if (node == RockNode)
	{
		for (int i = 0; i < m->GetSubMeshCount(); ++i) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, matTextures[i]);
			m->DrawSubMesh(i);
		}
		//node->Draw(*this);
	} 
	if (node == ShinNode)
	{
		DrawShinji();
		//node->Draw(*this);
	}

	for (auto it = node->GetChildIteratorStart(); it != node->GetChildIteratorEnd(); ++it) {
		DrawNode(*it);
	}
}

void Renderer::DrawShinji()
{
	//glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	BindShader(Anmshader);
	modelMatrix = Matrix4::Translation(heightmapSize * Vector3(0.5f, 0.7f, 0.3f)) * Matrix4::Scale(Vector3(100.0f, 100.0f, 100.0f));
	UpdateShaderMatrices();

	//glUniform1i(glGetUniformLocation(Meshshader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(lightShader->GetProgram(), "diffuseTex"), 0);
	glUniform4f(glGetUniformLocation(lightShader->GetProgram(), "ambientLightColor"), 0.7f, 0.6f, 1.0f, 1.0f);
	//glUniform1i(glGetUniformLocation(Anmshader->GetProgram(), "diffuseTex"), 0);
	vector < Matrix4 > frameMatrices;
	const Matrix4* invBindPose = Shinji->GetInverseBindPose();
	const Matrix4* frameData = Shinjianim->GetJointData(currentFrame);
	for (unsigned int i = 0; i < Shinji->GetJointCount(); ++i) {
		frameMatrices.emplace_back(frameData[i] * invBindPose[i]);

	}
	int j = glGetUniformLocation(Anmshader->GetProgram(), "joints");
	glUniformMatrix4fv(j, frameMatrices.size(), false, (float*)frameMatrices.data());
	for (int i = 0; i < Shinji->GetSubMeshCount(); ++i) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, ShinjiTextures[i]);
		Shinji->DrawSubMesh(i);
	}

}

void Renderer::DrawRain() {
	BindShader(rainShader);
	glUniform1f(glGetUniformLocation(rainShader->GetProgram(), "ambientStrength"), 0.8f);
	glUniform3fv(glGetUniformLocation(rainShader->GetProgram(), "lightColor"), 1, (float*)&nightLight->GetColour());

	//glViewport(0, 0, width / 2, height);
	glUniformMatrix4fv(glGetUniformLocation(rainShader->GetProgram(), "viewMatrix"), 1, false, (float*)&camera->BuildViewMatrix());
	glUniformMatrix4fv(glGetUniformLocation(rainShader->GetProgram(), "projMatrix"), 1, false, projMatrix.values);


	for (const auto& pos : rainPositions) {
		modelMatrix = Matrix4::Translation(pos) * Matrix4::Scale(Vector3(5.0f, 5.0f, 5.0f));
		glUniformMatrix4fv(glGetUniformLocation(rainShader->GetProgram(), "modelMatrix"), 1, false, modelMatrix.values);

		triangle->Draw();
	}

}