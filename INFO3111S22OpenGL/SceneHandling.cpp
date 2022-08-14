
#include <string>
#include <fstream>
#include "cVAOManager.h"
#include "cMesh.h"
#include <iostream>
#include <set>
#include "FlyCamera/cFlyCamera.h"
#include "cLoadTexture.h"

#include <stdio.h>  /* defines FILENAME_MAX */
#include <direct.h>
#define GetCurrentDir _getcwd

extern std::vector< cMesh* > g_vec_pMeshesToDraw;

extern glm::vec3 g_cameraEye;
extern cFlyCamera* g_pFlyCamera;

std::vector<std::string> diagram;

//get current path
std::string dir() {
	char cCurrentPath[FILENAME_MAX];

	if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath)))
	{
		return "ERROR";
	}

	cCurrentPath[sizeof(cCurrentPath) - 1] = '\0'; /* not really required */

	return cCurrentPath;
}

inline bool exists_test(const std::string& name) {
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}

bool LoadAllTheModels(std::string sceneFileName,
	cVAOManager* pVAOManager,
	unsigned int shaderProgramID,
	std::string& error)
{
	// Open the file that lists the models I'd like to load into the VAO
	std::ifstream sceneFile(sceneFileName.c_str());
	if (!sceneFile.good()) {
		error = "Error: couldn't find file " + sceneFileName;
		return false;
	}
	if (!sceneFile.is_open())
	{
		error = "Can't load file " + sceneFileName;
		return false;
	}

	// File is open at this point
	// How many types of models am I going to load? 
	// 1st number in the file is this number
	unsigned int numberOfModelTypesToLoad = 0;
	sceneFile >> numberOfModelTypesToLoad;

	//amt of files to load
	//file names
	unsigned int numberOfModelTypesLoaded = 0;
	do
	{
		std::string currentModelTypeFileName;
		sceneFile >> currentModelTypeFileName;

		sModelDrawInfo drawInfo;
		pVAOManager->LoadModelIntoVAO(currentModelTypeFileName, drawInfo, shaderProgramID);

		numberOfModelTypesLoaded++;

	} while (numberOfModelTypesLoaded < numberOfModelTypesToLoad);

	if (sceneFile.peek() != (int)sceneFile.eof()) {
		//amt of models to load
		//file names
		//x y z
		//rotation degree x rotation degree y rotation degree z
		// r g b a
		// nx ny nz <-- reminder that these mean the normals
		// tx ty <--texture x and y
		//scale
		//wireframe
		unsigned int numberOfModelsToLoad = 0;
		sceneFile >> numberOfModelsToLoad;
		std::string tmp = "";

		unsigned int numberOfModelsLoaded = 0;
		while (numberOfModelsLoaded < numberOfModelsToLoad)
		{
			cMesh* mesh = new cMesh();

			sceneFile >> tmp;
			if (tmp == "COMMENT_START") {
				while (tmp != "COMMENT_END") {
					sceneFile >> tmp;
				}
				sceneFile >> tmp;
			}
			else 
				mesh->meshFileName;
			
			if (tmp != "")mesh->meshFileName = tmp;
			else sceneFile >> mesh->meshFileName;
			if (mesh->meshFileName == "")break;
			
			if(!exists_test(dir() + "\\" + mesh->meshFileName)){
				error = "Error: couldn't find file " + mesh->meshFileName;
				std::cout << error << std::endl;
				return false;
			}
			
			sceneFile >> mesh->XYZLocation.x;
			sceneFile >> mesh->XYZLocation.y;
			sceneFile >> mesh->XYZLocation.z;
			sceneFile >> mesh->RGBA.r;
			sceneFile >> mesh->RGBA.g;
			sceneFile >> mesh->RGBA.b;
			sceneFile >> mesh->RGBA.a;
			sceneFile >> mesh->orientationEulerAngle.x;
			sceneFile >> mesh->orientationEulerAngle.y;
			sceneFile >> mesh->orientationEulerAngle.z;
			sceneFile >> mesh->overallScale;
			sceneFile >> tmp;

			/*
			* order is:
			* friendly
			* wire
			* texture
			*/
			if (tmp == "has_friendly") {
				sceneFile >> tmp;
				mesh->friendlyName = tmp;
				sceneFile >> tmp;
			}
			if (tmp == "is_wireframe") {
				mesh->bIsWireframe = true;
				sceneFile >> tmp;
			}
			if (tmp == "has_texture") {
				sceneFile >> mesh->texturePath;
				int txtAmt = 0;
				tmp = "";
				std::set<std::string> textures;

				for (int i = 0; i < mesh->MAXNUMTEXTURES; ++i) {
					sceneFile >> tmp;
					if (tmp == "texture_end")break;
					mesh->textures[i] = tmp;
					textures.insert(mesh->textures[i]);
					++txtAmt;
					tmp = "";
				}
				for (int i = 0; i < txtAmt; ++i) {
					sceneFile >> mesh->textureRatio[i];
				}
				mesh->usedTextures = txtAmt;
				tmp = "";
			}
			g_vec_pMeshesToDraw.push_back(mesh);
			++numberOfModelsLoaded;
		}
	}

	return true;
}

//https://stackoverflow.com/questions/686353/random-float-number-generation
float getRandBetween(float M, float N)
{
	return M + (rand() / (RAND_MAX / (N - M)));
}

bool SaveSceneToFile(std::string sceneFileName, std::string& error)
{
	std::ofstream sceneFile;
	sceneFile.open(sceneFileName);

	if (!sceneFile.good()) {
		error = "Can't find file " + sceneFileName;
		return false;
	}
	else if (!sceneFile.is_open())
	{
		error = "Can't load file " + sceneFileName;
		return false;
	}
	std::set< std::string > s;

	g_vec_pMeshesToDraw.erase(
		std::remove_if(g_vec_pMeshesToDraw.begin(), g_vec_pMeshesToDraw.end(), [&](cMesh* const& m) {
			return m->meshFileName == "";
			}),
		g_vec_pMeshesToDraw.end());

	for (cMesh* m : g_vec_pMeshesToDraw) {
		s.insert(m->meshFileName);
	}

	sceneFile << s.size() << "\n";
	for (std::string t : s) {
		sceneFile << t << "\n";
	}

	sceneFile << g_vec_pMeshesToDraw.size() << "\n";
	sceneFile << "COMMENT_START" << "\n";
	sceneFile << "XYZLocation.x   XYZLocation.y   XYZLocation.z" << "\n";
	sceneFile << "RGBA.x   RGBA.y   RGBA.z   RGBA.a" << "\n";
	sceneFile << "orientationEulerAngle.x   orientationEulerAngle.y   orientationEulerAngle.z" << "\n";
	sceneFile << "overallScale" << "\n";
	sceneFile << "EXTRA: " << "\n";
	sceneFile << "WALL AND FLOOR WIDTH IS 175" << "\n";
	sceneFile << "ARCH INSTEAD OF DOOR FOR NOW" << "\n";
	sceneFile << "EMPTY FLOOR INSTEAD OF STAIRS FOR NOW(#13 & #14)" << "\n";
	sceneFile << "COMMENT_END" << "\n";
	
	for (unsigned int index = 0; index != g_vec_pMeshesToDraw.size(); index++) {
		cMesh* pCurrentMesh = g_vec_pMeshesToDraw.at(index);
		sceneFile << pCurrentMesh->meshFileName << "\n";
		sceneFile << pCurrentMesh->XYZLocation.x << "   " << pCurrentMesh->XYZLocation.y
			<< "   " << pCurrentMesh->XYZLocation.z << "\n";
		sceneFile << pCurrentMesh->RGBA.r << "   " << pCurrentMesh->RGBA.g
			<< "   " << pCurrentMesh->RGBA.b << "   " << pCurrentMesh->RGBA.a << "\n";
		sceneFile << pCurrentMesh->orientationEulerAngle.x << "   " << pCurrentMesh->orientationEulerAngle.y
			<< "   " << pCurrentMesh->orientationEulerAngle.z << "\n";
		sceneFile << pCurrentMesh->overallScale << "\n";
		if (pCurrentMesh->bIsWireframe)sceneFile << "is_wireframe\n";
		if(pCurrentMesh->friendlyName != "")sceneFile << "has_friendly   " << pCurrentMesh->friendlyName << "\n";
		if (!pCurrentMesh->textures[0].empty()) {
			sceneFile << "has_texture\n";
			sceneFile << pCurrentMesh->texturePath << "\n";

			for (int i = 0; i < pCurrentMesh->usedTextures; ++i) {
				sceneFile << pCurrentMesh->textures[i] << "   ";
			}

			sceneFile << "\n";

			sceneFile << "texture_end\n";

			for (int i = 0; i < pCurrentMesh->usedTextures; ++i) {
				sceneFile << pCurrentMesh->textureRatio[i] << "   ";
			}
			sceneFile << "\n";
		}
	}

	error = "Can't load file " + sceneFileName;

	sceneFile.close();

	std::cout << "Saved info. to file" << std::endl;
	return true;
}

void SaveCamaeraToFile(std::string cameraFileName)
{
	std::ofstream file;
	file.open(cameraFileName);

	if (!file.good()) {
		std::cout << "Can't find file " + cameraFileName;
	}
	else if (!file.is_open())
	{
		std::cout << "Can't load file " + cameraFileName;
	}

	if (g_pFlyCamera != NULL) {
		file << g_pFlyCamera->eye.x << "   " << g_pFlyCamera->eye.y << "   " << g_pFlyCamera->eye.z << "\n";
	}
	else file << g_cameraEye.x << "   " << g_cameraEye.y << "   " << g_cameraEye.z << "\n";
	file.close();
}
 
glm::vec3 LoadCameraFromFile(std::string cameraFileName)
{
	std::ifstream file;
	file.open(cameraFileName);

	if (!file.good()) {
		std::cout << "Can't find file " + cameraFileName;
		return glm::vec3(0.0, 0.0, -4.0f);
	}
	else if (!file.is_open())
	{
		std::cout << "Can't load file " + cameraFileName;
		return glm::vec3(0.0, 0.0, -4.0f);
	}
	float x, y, z;
	file >> x >> y >> z;

	file.close();
	return glm::vec3(x, y, z);
}

// Returns 0 or NULL (or nullptr) if NOT found
cMesh* g_findObjectByFriendlyName(std::string theName)
{
	for (unsigned int index = 0; index != ::g_vec_pMeshesToDraw.size(); index++)
	{
		// Is this it? 
		cMesh* pTestObject = ::g_vec_pMeshesToDraw[index];

		if (pTestObject->friendlyName == theName)
		{
			return pTestObject;
		}
	}

	// Nope. It's not there. Sorry.
	return NULL;
}

bool LoadModelDiagram(std::string sceneFileName, std::string error) {
	if (exists_test(dir() + "\\" + sceneFileName)) {
		std::ifstream sceneFile(sceneFileName.c_str());
		std::string line;
		int numObjects = 0;
		while (line[0] != '_') {
			sceneFile >> line;
		}
		diagram.insert(diagram.begin(), line + "\n");
		while (numObjects < 9) {
			sceneFile >> line;
			diagram.insert(diagram.begin(), line + "\n");
			++numObjects;
		}
		if (diagram[0] != "") {
			return true;
		}
		return false;
	}
	else {
		std::cout << "Can't find file " << sceneFileName << "\n";
		return false;
	}
}

bool LoadModelDiagramAndModels(std::string sceneFileName,
	cVAOManager* pVAOManager,
	unsigned int shaderProgramID,
	std::string& error) 
{
	//if (!LoadModelDiagram(sceneFileName, error)) {
	//	return false;
	//}
	//
	//int total = 0;
	//int numObjects = 0;
	//float prevWallW = .0f;
	//float prevFloorW = .0f;
	//float prevWallH = .0f;
	//float prevFloorH = .0f;
	//float heightConstant= 0.f;

	//sModelDrawInfo drawInfo;

	//{
	//	std::string m1 = "assets/models/Dungeon_models/Floors/SM_Env_Dwarf_Floor_14.ply";
	//	std::string m2 = "assets/models/Dungeon_models/Walls/SM_Env_Dwarf_Wall_01.ply";
	//	std::string m3 = "assets/models/Dungeon_models/Walls/SM_Env_Dwarf_Wall_Archway_01.ply";
	//	std::string m4 = "assets/models/Dungeon_models/Walls/SM_Env_Dwarf_Wall_Window_01.ply";
	//	std::string m5 = "assets/models/Dungeon_models/Extra/SM_Bld_Dwarf_Building_Door_02_B.ply";
	//	std::string m6 = "assets/models/Dungeon_models/Extra/SM_Env_Dwarf_Narrow_Stairs_01.ply";

	//	pVAOManager->LoadModelIntoVAO(m1, drawInfo, shaderProgramID);
	//	pVAOManager->LoadModelIntoVAO(m2, drawInfo, shaderProgramID);
	//	pVAOManager->LoadModelIntoVAO(m3, drawInfo, shaderProgramID);
	//	pVAOManager->LoadModelIntoVAO(m4, drawInfo, shaderProgramID);
	//	pVAOManager->LoadModelIntoVAO(m5, drawInfo, shaderProgramID);
	//	pVAOManager->LoadModelIntoVAO(m6, drawInfo, shaderProgramID);
	//}

	//
	//
	//while (numObjects < diagram.size()) {
	//	std::string line = diagram[numObjects];
	//	for (unsigned i = 0; i < line.size(); ++i) {
	//		if (line[i] == '_') {

	//			//floor
	//			cMesh* pMesh = new cMesh();
	//			pMesh->meshFileName = "assets/models/Dungeon_models/Floors/SM_Env_Dwarf_Floor_14.ply";
	//			pMesh->XYZLocation = glm::vec3(-74.f, 10.f, -338.f);
	//			//floor width
	//			if (i > 0) {
	//				pMesh->XYZLocation.x = prevFloorW + 175;
	//				prevFloorW = pMesh->XYZLocation.x;
	//			}
	//			else {
	//				prevFloorW = pMesh->XYZLocation.x;
	//			}
	//			if(numObjects > 0)
	//				pMesh->XYZLocation.y += heightConstant;

	//			pMesh->RGBA = glm::vec4(1.f, 1.f, 1.f, 1.f);
	//			pMesh->orientationEulerAngle = glm::vec3(.0f, .0f, .0f);
	//			pMesh->overallScale = 0.35f;
	//			pMesh->friendlyName = "Level:" + std::to_string(numObjects + 1) + ",Floor:" + std::to_string(i + 1) +",ID=" + std::to_string(g_vec_pMeshesToDraw.size());
	//			pMesh->texturePath = "assets/textures/Dungeons";
	//			pMesh->textures[0] = "Dungeons_2_Texture_01_A_rotated_180_degrees.bmp";
	//			pMesh->textureRatio[0] = 1;
	//			pMesh->usedTextures = 1;
	//			g_vec_pMeshesToDraw.push_back(pMesh);



	//			//wall
	//			cMesh* pMesh2 = new cMesh();
	//			pMesh2->meshFileName = "assets/models/Dungeon_models/Walls/SM_Env_Dwarf_Wall_01.ply";
	//			pMesh2->XYZLocation = glm::vec3(-249.f, 3.5f, -509.f);
	//			//wall width
	//			if (i > 0) {
	//				pMesh2->XYZLocation.x = prevWallW + 175;
	//				prevWallW = pMesh2->XYZLocation.x;
	//			}
	//			else {
	//				prevWallW = pMesh2->XYZLocation.x;
	//			}
	//			if (numObjects > 0)
	//				pMesh2->XYZLocation.y += heightConstant;

	//			pMesh2->RGBA = glm::vec4(1.f, 1.f, 1.f, 1.f);
	//			pMesh2->orientationEulerAngle = glm::vec3(.0f, 3.15f, .0f);
	//			pMesh2->overallScale = 0.35f;
	//			pMesh2->friendlyName = "Level:" + std::to_string(numObjects + 1) + ",Wall:" + std::to_string(i + 1);
	//			pMesh2->texturePath = "assets/textures/Dungeons";
	//			pMesh2->textures[0] = "Dungeons_2_Texture_01_A_rotated_180_degrees.bmp";
	//			pMesh2->textureRatio[0] = 1;
	//			pMesh2->usedTextures = 1;
	//			g_vec_pMeshesToDraw.push_back(pMesh2);
	//			++total;
	//		}
	//		else if (line[i] == '.') {
	//			//wall
	//			cMesh* pMesh2 = new cMesh();
	//			pMesh2->meshFileName = "assets/models/Dungeon_models/Walls/SM_Env_Dwarf_Wall_01.ply";
	//			pMesh2->XYZLocation = glm::vec3(-249.f, 3.5f, -509.f);
	//			//wall width
	//			if (i > 0) {
	//				pMesh2->XYZLocation.x = prevWallW + 175;
	//				prevWallW = pMesh2->XYZLocation.x;
	//			}
	//			else {
	//				prevWallW = pMesh2->XYZLocation.x;
	//			}
	//			
	//			if (numObjects > 0) {
	//				pMesh2->XYZLocation.y += heightConstant;
	//			}
	//			
	//			pMesh2->RGBA = glm::vec4(1.f, 1.f, 1.f, 1.f);
	//			pMesh2->orientationEulerAngle = glm::vec3(.0f, 3.15f, .0f);
	//			pMesh2->overallScale = 0.35f;
	//			pMesh2->friendlyName = "Level:" + std::to_string(numObjects + 1) + ",Wall:" + std::to_string(i + 1) + ",ID=" + std::to_string(g_vec_pMeshesToDraw.size());;
	//			pMesh2->texturePath = "assets/textures/Dungeons";
	//			pMesh2->textures[0] = "Dungeons_2_Texture_01_A_rotated_180_degrees.bmp";
	//			pMesh2->textureRatio[0] = 1;
	//			pMesh2->usedTextures = 1;
	//			g_vec_pMeshesToDraw.push_back(pMesh2);
	//			++total;
	//		}
	//		else if (line[i] == '\n') { 
	//			heightConstant += 175.f; 
	//		}
	//		else if (line[i] == 'd') {
	//			//floor
	//			cMesh* pMesh = new cMesh();
	//			pMesh->meshFileName = "assets/models/Dungeon_models/Floors/SM_Env_Dwarf_Floor_14.ply";
	//			pMesh->XYZLocation = glm::vec3(-74.f, 10.f, -338.f);
	//			//floor width
	//			if (i > 0) {
	//				if(heightConstant > 0)
	//					pMesh->XYZLocation.x = prevFloorW + 175;
	//				else
	//					pMesh->XYZLocation.x = prevFloorW + 175;
	//				prevFloorW = pMesh->XYZLocation.x;
	//			}
	//			else {
	//				prevFloorW = pMesh->XYZLocation.x;
	//			}
	//			if (numObjects > 0)
	//				pMesh->XYZLocation.y += heightConstant;

	//			pMesh->RGBA = glm::vec4(1.f, 1.f, 1.f, 1.f);
	//			pMesh->orientationEulerAngle = glm::vec3(.0f, .0f, .0f);
	//			pMesh->overallScale = 0.35f;
	//			pMesh->friendlyName = "Level:" + std::to_string(numObjects + 1) + ",DoorFloor:" + std::to_string(i + 1);
	//			pMesh->texturePath = "assets/textures/Dungeons";
	//			pMesh->textures[0] = "Dungeons_2_Texture_01_A_rotated_180_degrees.bmp";
	//			pMesh->textureRatio[0] = 1;
	//			pMesh->usedTextures = 1;
	//			g_vec_pMeshesToDraw.push_back(pMesh);



	//			//wall
	//			cMesh* pMesh2 = new cMesh();
	//			pMesh2->meshFileName = "assets/models/Dungeon_models/Extra/SM_Bld_Dwarf_Building_Door_02_B.ply";
	//			pMesh2->XYZLocation = glm::vec3(-249.f, 3.5f, -504.f);
	//			//wall width
	//			if (i > 0) {
	//				pMesh2->XYZLocation.x = prevWallW + 175;
	//				prevWallW = pMesh2->XYZLocation.x;
	//				pMesh2->XYZLocation.x += 178.5;
	//			}
	//			else {
	//				prevWallW = pMesh2->XYZLocation.x;
	//			}
	//			if (numObjects > 0)
	//				pMesh2->XYZLocation.y += heightConstant;

	//			pMesh2->RGBA = glm::vec4(1.f, 1.f, 1.f, 1.f);
	//			pMesh2->orientationEulerAngle = glm::vec3(.0f, 3.15f, .0f);
	//			pMesh2->overallScale = 1.f;
	//			pMesh2->friendlyName = "Level:" + std::to_string(numObjects + 1) + ",Door:" + std::to_string(i + 1);
	//			pMesh2->texturePath = "assets/textures/Dungeons";
	//			pMesh2->textures[0] = "Dungeons_2_Texture_01_A_rotated_180_degrees.bmp";
	//			pMesh2->textureRatio[0] = 1;
	//			pMesh2->usedTextures = 1;
	//			g_vec_pMeshesToDraw.push_back(pMesh2);
	//			++total;
	//		}
	//		else if (line[i] == 's') {
	//			prevFloorW = prevFloorW + 175;

	//			//wall
	//			cMesh* pMesh = new cMesh();
	//			pMesh->meshFileName = "assets/models/Dungeon_models/Walls/SM_Env_Dwarf_Wall_01.ply";
	//			pMesh->XYZLocation = glm::vec3(-249.f, 3.5f, -509.f);
	//			//wall width
	//			if (i > 0) {
	//				pMesh->XYZLocation.x = prevWallW + 175;
	//				prevWallW = pMesh->XYZLocation.x;
	//			}
	//			else {
	//				prevWallW = pMesh->XYZLocation.x;
	//			}
	//			if (numObjects > 0)
	//				pMesh->XYZLocation.y += heightConstant;

	//			pMesh->RGBA = glm::vec4(1.f, 1.f, 1.f, 1.f);
	//			pMesh->orientationEulerAngle = glm::vec3(.0f, 3.15f, .0f);
	//			pMesh->overallScale = 0.35f;
	//			pMesh->friendlyName = "Level:" + std::to_string(numObjects + 1) + ",StairWall:" + std::to_string(i + 1);
	//			pMesh->texturePath = "assets/textures/Dungeons";
	//			pMesh->textures[0] = "Dungeons_2_Texture_01_A_rotated_180_degrees.bmp";
	//			pMesh->textureRatio[0] = 1;
	//			pMesh->usedTextures = 1;
	//			g_vec_pMeshesToDraw.push_back(pMesh);
	//			++total;



	//			//stair
	//			cMesh* pMesh2 = new cMesh();
	//			pMesh2->meshFileName = "assets/models/Dungeon_models/Extra/SM_Env_Dwarf_Narrow_Stairs_01.ply";
	//			pMesh2->XYZLocation = glm::vec3(-249.f, 3.5f, -509.f);
	//			//wall width
	//			if (i > 0) {
	//				pMesh2->XYZLocation.x = prevFloorW + 175;
	//				prevFloorW = pMesh2->XYZLocation.x;
	//			}
	//			else {
	//				prevFloorW = pMesh2->XYZLocation.x;
	//			}
	//			if (numObjects > 0)
	//				pMesh2->XYZLocation.y += heightConstant;

	//			pMesh2->RGBA = glm::vec4(1.f, 1.f, 1.f, 1.f);
	//			pMesh2->orientationEulerAngle = glm::vec3(.0f, glm::radians(270.f), .0f);
	//			pMesh2->overallScale = 0.35f;
	//			pMesh2->friendlyName = "Level:" + std::to_string(numObjects + 1) + ",Stair:" + std::to_string(i + 1);
	//			pMesh2->texturePath = "assets/textures/Dungeons";
	//			pMesh2->textures[0] = "SM_Env_Dwarf_Narrow_Stairs_01.ply";
	//			pMesh2->textureRatio[0] = 1;
	//			pMesh2->usedTextures = 1;
	//			g_vec_pMeshesToDraw.push_back(pMesh2);
	//			++total;
	//		}
	//		else if (line[i] == 'w') {
	//			//wall
	//			cMesh* pMesh2 = new cMesh();
	//			pMesh2->meshFileName = "assets/models/Dungeon_models/Walls/SM_Env_Dwarf_Wall_Window_01.ply";
	//			pMesh2->XYZLocation = glm::vec3(-249.f, 3.5f, -509.f);
	//			//wall width
	//			if (i > 0) {
	//				pMesh2->XYZLocation.x = prevWallW + 175;
	//				prevWallW = pMesh2->XYZLocation.x;
	//			}
	//			else {
	//				prevWallW = pMesh2->XYZLocation.x;
	//			}

	//			if (numObjects > 0) {
	//				pMesh2->XYZLocation.y += heightConstant;
	//			}

	//			pMesh2->RGBA = glm::vec4(1.f, 1.f, 1.f, 1.f);
	//			pMesh2->orientationEulerAngle = glm::vec3(.0f, 3.15f, .0f);
	//			pMesh2->overallScale = 0.35f;
	//			pMesh2->friendlyName = "Level:" + std::to_string(numObjects + 1) + ",Wall:" + std::to_string(i + 1);
	//			pMesh2->texturePath = "assets/textures/Dungeons";
	//			pMesh2->textures[0] = "Dungeons_2_Texture_01_A_rotated_180_degrees.bmp";
	//			pMesh2->textureRatio[0] = 1;
	//			pMesh2->usedTextures = 1;
	//			g_vec_pMeshesToDraw.push_back(pMesh2);
	//			++total;
	//		}

	//	}



	//	++numObjects;
	//	++total;
	//}
	return false;
}

void RotateFloor() {
	for(cMesh* pMesh : g_vec_pMeshesToDraw) {
		if (pMesh->friendlyName.find("Level:4,Floor:") != std::string::npos || pMesh->friendlyName.find("Level:4,DoorFloor:") != std::string::npos) {
			pMesh->meshFileName = "assets/models/Dungeon_models/Floors/SM_Env_Dwarf_Floor_19.ply";
			pMesh->orientationEulerAngle.y += glm::radians(90.f);
			pMesh->XYZLocation.z -= 170.f;
		}
		else if (pMesh->friendlyName.find("Level:10,Floor:") != std::string::npos || pMesh->friendlyName.find("Level:10,DoorFloor:") != std::string::npos) {
			pMesh->meshFileName = "assets/models/Dungeon_models/Floors/SM_Env_Dwarf_Floor_19.ply";
			pMesh->orientationEulerAngle.y += glm::radians(90.f);
			pMesh->XYZLocation.z -= 170.f;
		}
	}
}

bool g_findObjectByFriendlyName(std::string theName, cMesh* theObjectIFound);
