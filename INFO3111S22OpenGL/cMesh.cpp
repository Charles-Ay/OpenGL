#include "cMesh.h"

static unsigned int m_NextID;	// = 0;
static unsigned int target = 16;


cMesh::cMesh()
{
	// 4th value is the alpha transparency (blend) value.
	// 0.0 = transparent (invisible)
	// 1.0 = opaque ('solid')
	this->RGBA = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	this->XYZLocation = glm::vec3(0.0f, 0.0f, 0.0f);
	this->orientationEulerAngle = glm::vec3(0.0f, 0.0f, 0.0f);
	this->overallScale = 1.0f;
	this->bIsWireframe = false;

	this->bIsVisible = true;
	this->bIsNotLit = false;
	this->bDisableBackFaceCulling = false;
	this->bDisableDepthBuffer = false;

	this->bUseModelFileColours = false;

	this->uniqueObjectID = cMesh::m_NextID++;

	for (float& ratio : textureRatio)
	{
		ratio = 0.0f;
	}
}

/*static*/
const unsigned int STARTING_MESH_UNIQUE_ID = 1000;
unsigned int cMesh::m_NextID = STARTING_MESH_UNIQUE_ID;


void cMesh::setAlphaTransparency(float newAlpha)
{
	this->RGBA.a = newAlpha;
}
float cMesh::getAlphaTransparency(void)
{
	return this->RGBA.a;
}

unsigned int cMesh::getUniqueObjectID(void)
{
	return this->uniqueObjectID;
}

int cMesh::getTarget()
{
	return target;
}

void cMesh::nextTarget(std::vector< cMesh* > &g_vec_pMeshesToDraw)
{
	if (target + 1 < g_vec_pMeshesToDraw.size())++target;
	else target = 0;
}

void cMesh::previousTarget(std::vector< cMesh* >& g_vec_pMeshesToDraw)
{
	if (target - 1 >= 0)--target;
	else target = g_vec_pMeshesToDraw.size() -1;
}

std::string cMesh::getFriendlyByID(unsigned int id, std::vector< cMesh* >& g_vec_pMeshesToDraw)
{
	for (int i = 0; i < g_vec_pMeshesToDraw.size(); ++i) {
		if (i == id) {
			return g_vec_pMeshesToDraw[i]->friendlyName;
		}
	}
	return "";
}
