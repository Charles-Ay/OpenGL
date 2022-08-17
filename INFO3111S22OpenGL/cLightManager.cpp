#include "cLightManager.h"
#include <fstream>
#include <sstream>
#include <vector>
#include "cVAOManager.h"
#include "cMesh.h"
#include <GLFW/glfw3.h>

std::string cLightManager::LIGHT_OBJECT_NAME;
int cLightManager::numberOfUsedLights = 0;
extern std::vector< cMesh* > g_vec_pMeshesToDraw;
extern unsigned int currentLight;

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
	for (int i = 0; i < numberOfUsedLights; ++i) {
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

void cLightManager::NextLight()
{
	if(currentLight < numberOfUsedLights - 1)
		++currentLight;
	else
		currentLight = 0;
}

void cLightManager::PreviousLight()
{
	if (currentLight == 0) {
		currentLight = numberOfUsedLights - 1;
	}
	else --currentLight;
}

void cLightManager::GoToLastLight()
{
	currentLight = numberOfUsedLights -1;
}

void cLightManager::RandomizeLightAttens()
{
	//for (sLightDescription light : theLights) {
	//	if (light.diffuse.x < 1.0f && light.diffuse.y < 1.0f) {
	//		int plusMinus = rand() % 2;
	//		float randFloat = rand() % 2 + 1;// 0 - 2
	//		randFloat /= 100;
	//		if (plusMinus == 0) {
	//			light.atten.x = light.atten.x - randFloat;
	//		}
	//		else {
	//			light.atten.x = light.atten.x + randFloat;
	//		}
	//			
	//	}
	//}

	for (int i = 0; i < numberOfUsedLights; ++i) {
		sLightDescription li = theLights[i];
		if (li.diffuse.x < 1.0f && li.diffuse.y < 1.0f && i < 13) {
			int plusMinus = rand() % 2;
			float randFloat = rand() % 2 + 1;// 0 - 2
			randFloat /= 5;
			if (plusMinus == 0) {
				if(li.atten.x > 0.0f)
					li.atten.x = li.atten.x - randFloat;
				else {
					li.atten.x = li.atten.x + randFloat;
				}
			}
			else {
				if(li.atten.x < 3.0f)
					li.atten.x = li.atten.x + randFloat;
				else {
					li.atten.x = li.atten.x - randFloat;
				}
				li.atten.x = li.atten.x + randFloat;
			}

		}
		theLights[i] = li;
	}
}

void cLightManager::DayNightCycle(int lastFrameTime)
{
	//start pos -1010.81   1194   1105.4
	if (dayStartPos == glm::vec4())
		dayStartPos = theLights[0].position;
	double cycleTime = 20.0;
	if (bDayNight) {
		thisFrameTime = glfwGetTime();
		double delta = thisFrameTime - lastFrameTime;
		if (delta < cycleTime) {
			if (!hitPeak) {
				if (theLights[0].position.y < 3493) {
					theLights[0].position.y += 2;
				}
				if (theLights[0].position.x < 1460) {
					theLights[0].position.x += 2;
				}
				else hitPeak = true;
			}
			else {
				if (theLights[0].position.y > 1008) {
					theLights[0].position.y -= 2;
				}
				if (theLights[0].position.x < 3822) {
					theLights[0].position.x += 2;
				}
				else {
					hitPeak = false;
					theLights[0].position = dayStartPos;
				}
			}
		}
	}
}

void sLightDescription::ChangeTargetToLookAt()
{
	if (useTarget) {
		//if (currentTarget < g_vec_pMeshesToDraw.size() - 1) {
		//	++currentTarget;
		//}
		//else {
		//	currentTarget = 0;
		//}

		while (static_cast<unsigned long long>(currentTarget) + 1 < g_vec_pMeshesToDraw.size())
		{
			//not found string
			if (g_vec_pMeshesToDraw[currentTarget + 1]->friendlyName.find("Torch") == std::string::npos) {
				currentTarget++;
				//if (target == 0) {
				//	prevColor = g_vec_pMeshesToDraw[target]->RGBA;
				//	prevTarget = target;
				//	g_vec_pMeshesToDraw[target]->RGBA = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
				//}
				//else {
				//	g_vec_pMeshesToDraw[prevTarget]->RGBA = prevColor;
				//	prevColor = g_vec_pMeshesToDraw[target]->RGBA;
				//	prevTarget = target;
				//	g_vec_pMeshesToDraw[target]->RGBA = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
				//}
				//g_vec_pMeshesToDraw[target]->RGBA = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
			}
			else {
				//std::cout << "target: " << target << std::endl;
				currentTarget++;
				break;
			}
		}
		if (static_cast<unsigned long long>(currentTarget) + 1 > g_vec_pMeshesToDraw.size()) {
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