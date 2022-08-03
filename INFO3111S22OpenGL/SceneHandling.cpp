
#include <string>
#include <fstream>
#include "cVAOManager.h"
#include "cMesh.h"
#include <iostream>
#include <set>
#include "FlyCamera/cFlyCamera.h"

extern std::vector< cMesh* > g_vec_pMeshesToDraw;

extern glm::vec3 g_cameraEye;
extern cFlyCamera* g_pFlyCamera;

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
			if (tmp != "")mesh->meshFileName = tmp;
			else sceneFile >> mesh->meshFileName;
			if (mesh->meshFileName == "")break;
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
			if (tmp == "is_wireframe") {
				mesh->bIsWireframe = true;
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