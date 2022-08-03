#include "cLightManager.h"
#include <fstream>
#include <sstream>
#include <vector>
#include "cVAOManager.h"
#include "cMesh.h"

std::string cLightManager::LIGHT_OBJECT_NAME;
int cLightManager::numberOfUsedLights = 0;
extern std::vector< cMesh* > g_vec_pMeshesToDraw;

bool cLightManager::SaveLightConfigToFile(cLightManager& lightManager, std::string fileName, std::string& error)
{
	std::ofstream file;
	file.open(fileName);

	if (!file.good()) {
		error = "Can't find file " + fileName;
		return false;
	}
	else if (!file.is_open())
	{
		error = "Can't load file " + fileName;
		return false;
	}

	file << numberOfUsedLights << "\n";
	file << LIGHT_OBJECT_NAME << "\n";
	for (unsigned int i = 0; i < numberOfUsedLights; ++i) {
		file << lightManager.theLights[i].position.x << "   " << lightManager.theLights[i].position.y 
			<< "   " << lightManager.theLights[i].position.z << "\n";
		
		file << lightManager.theLights[i].diffuse.r << "   " << lightManager.theLights[i].diffuse.g
			<< "   " << lightManager.theLights[i].diffuse.b << "   " << lightManager.theLights[i].diffuse.a << "\n";

		file << lightManager.theLights[i].specular.r << "   " << lightManager.theLights[i].specular.g
			<< "   " << lightManager.theLights[i].specular.b << "   " << lightManager.theLights[i].specular.a << "\n";

		file << lightManager.theLights[i].atten.r << "   " << lightManager.theLights[i].atten.g
			<< "   " << lightManager.theLights[i].atten.b << "   " << lightManager.theLights[i].atten.a << "\n";
		
		file << lightManager.theLights[i].direction.r << "   " << lightManager.theLights[i].direction.g
			<< "   " << lightManager.theLights[i].direction.b << "   " << lightManager.theLights[i].direction.a << "\n";
		
		file << lightManager.theLights[i].param1.x << "   " << lightManager.theLights[i].param1.y
			<< "   " << lightManager.theLights[i].param1.z << "   " << lightManager.theLights[i].param1.w << "\n";
		
		file << lightManager.theLights[i].param2.x << "\n";
		file << lightManager.theLights[i].currentTarget << "\n";
	}
	file.close();
	return true;
}

//finsh l8r
bool cLightManager::LoadLightConfigFromFile(cLightManager &lightManager, cVAOManager* pVAOManager, unsigned int shaderProgramID, std::string fileName, std::string &error)
{
	std::ifstream file(fileName.c_str());
	if (!file.good()) {
		error = "Error: couldn't find file " + fileName;
		return false;
	}
	if (!file.is_open())
	{
		error = "Can't load file " + fileName;
		return false;
	}
	
	if (file.is_open())
	{
		//number of lights
		unsigned int numLights;
		file >> numLights;
		file >> LIGHT_OBJECT_NAME;
		
		sModelDrawInfo drawInfo;
		pVAOManager->LoadModelIntoVAO(LIGHT_OBJECT_NAME, drawInfo, shaderProgramID);
		
		for (unsigned int i = 0; i < numLights; ++i) {

			std::string discard;
			float 
				posx, posy, posz, //position
				defr, defg, defb, defa, //defuse
				specr, specg, specb, speca, //specular
				attenr, atteng, attenb, attena, //attinuation
				dirr, dirg, dirb, dira, //direction
				param1x, param1y, param1z, param1w,
				param2; //intesity
			
			file >> posx >> posy >> posz;
			lightManager.theLights[i].position = glm::vec4(posx, posy, posz, 1.0f);
			file >> defr >> defg >> defb >> defa;
			lightManager.theLights[i].diffuse = glm::vec4(defr, defg, defb, defa);//the diffuse color of the light
			file >> specr >> specg >> specb >> speca;
			lightManager.theLights[i].specular= glm::vec4(specr, specg, specb, speca);
			file >> attenr >> atteng >> attenb >> attena;
			lightManager.theLights[i].atten = glm::vec4(attenr, atteng, attenb, attena);
			file >> dirr >> dirg >> dirb >> dira;
			lightManager.theLights[i].direction = glm::vec4(dirr, dirg, dirb, dira);
			file >> param1x >> param1y >> param1z >> param1w;
			lightManager.theLights[i].param1 = glm::vec4(param1x, param1y, param1z, 0);
			file >> param2;
			lightManager.theLights[i].param2 = glm::vec4(param2, 0, 0, 0);
			file >> lightManager.theLights[i].currentTarget;
			++numberOfUsedLights;
		}
	}
	file.close();
	return true;
}

void sLightDescription::ChangeTargetToLookAt()
{
	if (useTarget) {
		if (currentTarget < g_vec_pMeshesToDraw.size() - 1) {
			++currentTarget;
		}
		else {
			currentTarget = 0;
		}
	}
}

void sLightDescription::MoveToTarget()
{
	if (useTarget) {
		position = glm::vec4(g_vec_pMeshesToDraw[currentTarget]->XYZLocation.x, position.y, g_vec_pMeshesToDraw[currentTarget]->XYZLocation.z, 1.0f);
	}
}