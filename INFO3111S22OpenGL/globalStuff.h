#pragma once

// This is because glad has to be compiled BEFORE glfw

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <vector>

// Also the mesh stuff
#include "cMesh.h"
#include "cVAOManager.h"
#include "cLightManager.h"
#include "FlyCamera/cFlyCamera.h"

// Anything in here is drawn in the scene
extern std::vector< cMesh* > g_vec_pMeshesToDraw;

extern cLightManager* pTheLightManager;

extern bool g_EnableDebugLightSpeheres;
extern bool bUseFlyCamera;	// = true

extern glm::vec3 g_cameraEye;
extern cFlyCamera* g_pFlyCamera;
extern unsigned int currentLight;

const std::string sceneFileName = "assets/saves/SaveFile.txt";
const std::string lightFileName = "assets/saves/LightConfigSaveFile.txt";
const std::string cameraFileName = "assets/saves/CameraSaveFile.txt";


bool LoadModelDiagram(std::string sceneFileName, std::string error);

bool LoadAllTheModels(std::string sceneFileName,
	cVAOManager* pVAOManager,
	unsigned int shaderProgramID,
	std::string& error);

bool LoadModelDiagramAndModels(std::string sceneFileName,
	cVAOManager* pVAOManager,
	unsigned int shaderProgramID,
	std::string& error);

bool SaveSceneToFile(std::string sceneFileName, std::string& error);

void SaveCamaeraToFile(std::string cameraFileName);
glm::vec3 LoadCameraFromFile(std::string cameraFileName);

void DrawObject(cMesh* pCurrentMesh, GLuint shaderProgramNumber, cVAOManager* pVAOManager,
				glm::mat4 matModel, glm::mat4 matView, glm::mat4 matProjection);


float getRandBetween(float M, float N);

// Returns 0 or NULL (or nullptr) if NOT found
cMesh* g_findObjectByFriendlyName(std::string theName);
bool g_findObjectByFriendlyName(std::string theName, cMesh* theObjectIFound);


cMesh* g_findObjectByUniqueID(unsigned int theID);

void UpdateEachFrame(double deltaTime);