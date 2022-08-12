#pragma once

#include <string>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <vector>
#include <set>

// This represents a 3D model we'd like to draw
class cMesh
{
public:
	cMesh();
	std::string friendlyName;

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

	static const unsigned int MAXNUMTEXTURES = 8;
	std::string textures[MAXNUMTEXTURES];
	std::string texturePath;

	float textureRatio[MAXNUMTEXTURES];
	int usedTextures;

	unsigned int getUniqueObjectID(void);

	static int getTarget();

	static void nextTarget(std::vector< cMesh* > &g_vec_pMeshesToDraw);
	static void previousTarget(std::vector< cMesh* >& g_vec_pMeshesToDraw);

	static std::string getFriendlyByID(unsigned int id, std::vector< cMesh* >& g_vec_pMeshesToDraw);
private:
	static unsigned int m_NextID;	// = 0;
	unsigned int uniqueObjectID;

};
