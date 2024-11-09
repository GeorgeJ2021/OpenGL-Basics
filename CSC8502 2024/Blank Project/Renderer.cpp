#include "Renderer.h"
#include "../nclgl/Light.h"
#include "../nclgl/Heightmap.h"
#include "../nclgl/Shader.h"
#include "../nclgl/Camera.h"

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
	quad = Mesh::GenerateQuad();
	heightMap = new HeightMap(TEXTUREDIR "noise4.png");
	//waterTex = SOIL_load_OGL_texture(TEXTUREDIR "redfanta.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	earthTex = SOIL_load_OGL_texture(TEXTUREDIR "sand.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	earthBump = SOIL_load_OGL_texture(TEXTUREDIR "sandnormal.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	waterBump = SOIL_load_OGL_texture(TEXTUREDIR "waterbump.PNG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	//cubeMap = SOIL_load_OGL_cubemap(TEXTUREDIR "cubemap_4.png", TEXTUREDIR "cubemap_5.png", TEXTUREDIR "cubemap_up.png", TEXTUREDIR "cubemap_down.png",
		//TEXTUREDIR "cubemap_1.png", TEXTUREDIR "cubemap_0.png", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

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
	if (!reflectShader->LoadSuccess() || !skyboxShader->LoadSuccess() || !lightShader->LoadSuccess()) {
		return;
	}
	Vector3 map = Vector3(0.5f, 1.0f, 0.4f);
	Vector3 heightmapSize = heightMap->GetHeightmapSize();
	camera = new Camera(0.0f, -180.0f, heightmapSize * map);
	camera->settrack(map, heightmapSize);

	//light = new Light(heightmapSize * Vector3(1.5f, 4.0f, 0.5f), Vector4(0.8, 0.6, 1, 1), heightmapSize.x * 2);

	dayLight = new Light(heightmapSize * Vector3(0.5f, 10.0f, 0.5f), Vector4(1.0, 1.0, 1.0, 1), heightmapSize.x * 10.0f);
	nightLight = new Light(heightmapSize * Vector3(1.0f, 4.0f, 0.5f), Vector4(0.5, 0.6, 0.8, 1), heightmapSize.x * 2.0f);

	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	waterRotate = 0.0f;
	waterCycle = 0.0f;
	init = true;

}

void Renderer::ToggleScene() {
	change = !change;
}

Renderer ::~Renderer(void) {
	delete camera;
	delete heightMap;
	delete quad;

	glDeleteTextures(1, &daySkybox);
	glDeleteTextures(1, &nightSkybox);
	glDeleteTextures(1, &dayWaterTex);
	glDeleteTextures(1, &nightWaterTex);

	delete dayLight;
	delete nightLight;
	delete reflectShader;
	delete skyboxShader;
	delete lightShader;
	delete light;
}

void Renderer::UpdateScene(float dt) {
	camera->UpdateCamera(dt);
	viewMatrix = camera->BuildViewMatrix();
	waterRotate += dt * 2.0f;
	waterCycle += dt * 0.25f;

}

void Renderer::RenderScene() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	DrawSkybox();
	DrawHeightmap();
	DrawWater();
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

/*
void Renderer::LoadScene1() {
	waterTex =SOIL_load_OGL_texture(TEXTUREDIR "water.TGA", SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_MIPMAPS);
	earthTex = SOIL_load_OGL_texture(TEXTUREDIR "Barren Reds.JPG", SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	earthBump = SOIL_load_OGL_texture(TEXTUREDIR "Barren RedsDOT3.JPG", SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	cubeMap = SOIL_load_OGL_cubemap(TEXTUREDIR "rusted_west.jpg", TEXTUREDIR "rusted_east.jpg",TEXTUREDIR "rusted_up.jpg", TEXTUREDIR "rusted_down.jpg",
			TEXTUREDIR "rusted_south.jpg", TEXTUREDIR "rusted_north.jpg",SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);
	if(!earthTex || !earthBump || !cubeMap || !waterTex) {
		return;
	}
}

void Renderer::LoadScene2() {
	waterTex = SOIL_load_OGL_texture(TEXTUREDIR "redfanta.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	earthTex = SOIL_load_OGL_texture(TEXTUREDIR "sand.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	earthBump = SOIL_load_OGL_texture(TEXTUREDIR "sandnormal.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	waterBump = SOIL_load_OGL_texture(TEXTUREDIR "waterbump.PNG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	cubeMap = SOIL_load_OGL_cubemap(TEXTUREDIR "cubemap_4.png", TEXTUREDIR "cubemap_5.png", TEXTUREDIR "cubemap_up.png", TEXTUREDIR "cubemap_down.png",
		TEXTUREDIR "cubemap_1.png", TEXTUREDIR "cubemap_0.png", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);
	if (!earthTex || !earthBump || !cubeMap || !waterTex) {
		return;

	}
} */