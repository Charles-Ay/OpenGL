#pragma once

#include <glm/glm.hpp>
#include <glm/vec4.hpp>
#include "cVAOManager.h"
#include <string>
#include "cLightHelper.h"

struct sLightDescription
{
	sLightDescription()
	{
		this->position = glm::vec4(0.0f);
		this->diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		this->specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		this->atten = glm::vec4(0.0f);
		this->direction = glm::vec4(0.0f);
		this->param1 = glm::vec4(0.0f);
		this->param2 = glm::vec4(0.0f);
	}

	glm::vec4 position;
	glm::vec4 diffuse;
	glm::vec4 specular;	// rgb = highlight colour, w = power
	glm::vec4 atten;		// x = constant, y = linear, z = quadratic, w = DistanceCutOff
	glm::vec4 direction;	// Spot, directional lights
	glm::vec4 param1;	// x = lightType, y = inner angle, z = outer angle, w = TBD
						// 0 = pointlight
						// 1 = spot light
						// 2 = directional light
	glm::vec4 param2;	// x = 0 for off, 1 for on

	void ChangeTargetToLookAt();
	void MoveToTarget();
	bool useTarget = false;

	int currentTarget = 0;
	
};


class cLightManager
{
public:
	cLightManager() {}
	static const unsigned int NUMBEROFLIGHTS = 5;
	static int numberOfUsedLights;
	//the model that is used for lighting
	static std::string LIGHT_OBJECT_NAME;
	sLightDescription theLights[NUMBEROFLIGHTS];

	// Maybe you add stuff like this:
	void TurnOnLight(unsigned int lightIndexNumber);
	void TurnAllTheLightsOff(void);
	bool SaveLightConfigToFile(cLightManager& lightManager, std::string fileName, std::string& error);
	bool LoadLightConfigFromFile(cLightManager& lightManager, cVAOManager* pVAOManager, unsigned int shaderProgramID, std::string fileName, std::string& error);
};
