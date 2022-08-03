#pragma once

#include <string>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <vector>

// This represents a 3D model we'd like to draw
class cMesh
{
public:
	cMesh();
	std::string meshFileName;
	// These are from 0.0 to 1.0f (1.0f = white, 0.0f = black)
	glm::vec4 RGBA;		// Red, Green, Blue, Alpha
	glm::vec3 XYZLocation;
	glm::vec3 orientationEulerAngle;
	float overallScale;		
	// 
	bool bIsWireframe;
	// 
	bool bIsVisible;
	bool bIsNotLit;					// Just be the vertex colours
	bool bDisableBackFaceCulling;	// Will show both front and back
	bool bDisableDepthBuffer;		// ON - always be seen, otherwise it can be blocked by other objects

	bool bUseModelFileColours;

	void setAlphaTransparency(float newAlpha);
	float getAlphaTransparency(void);

	std::vector< cMesh* > vec_pChildMeshes;


	unsigned int getUniqueObjectID(void);
private:
	static unsigned int m_NextID;	// = 0;
	unsigned int uniqueObjectID;
};
