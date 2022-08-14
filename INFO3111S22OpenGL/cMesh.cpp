#include "cMesh.h"
#include <iostream>

static unsigned int m_NextID;	// = 0;
static unsigned int target = 314;//180
static glm::vec4 prevColor;
static unsigned int prevTarget;


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
	if (target + 1 < g_vec_pMeshesToDraw.size()) {
		target++;
	}
	else {
		target = 0;
	}
	
	//while (static_cast<unsigned long long>(target) + 1 < g_vec_pMeshesToDraw.size())
	//{
	//	//not found string
	//	if (g_vec_pMeshesToDraw[target + 1]->friendlyName.find(",Stair:") == std::string::npos ) {
	//		target++;
	//		//if (target == 0) {
	//		//	prevColor = g_vec_pMeshesToDraw[target]->RGBA;
	//		//	prevTarget = target;
	//		//	g_vec_pMeshesToDraw[target]->RGBA = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	//		//}
	//		//else {
	//		//	g_vec_pMeshesToDraw[prevTarget]->RGBA = prevColor;
	//		//	prevColor = g_vec_pMeshesToDraw[target]->RGBA;
	//		//	prevTarget = target;
	//		//	g_vec_pMeshesToDraw[target]->RGBA = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	//		//}
	//		//g_vec_pMeshesToDraw[target]->RGBA = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	//	}
	//	else {
	//		//std::cout << "target: " << target << std::endl;
	//		target++;
	//		break;
	//	}

	//}
	//if (target + 1 > g_vec_pMeshesToDraw.size()) {
	//	target = 0;
	//}
	//std::cout << "target: " << target << std::endl;
}

void cMesh::previousTarget(std::vector< cMesh* >& g_vec_pMeshesToDraw)
{
	if (target == 0) {
		target = g_vec_pMeshesToDraw.size() - 1;
	}
	else {
		target--;
	}
	//while (static_cast<unsigned long long>(target) + 1 < g_vec_pMeshesToDraw.size())
	//{
	//	//not found string
	//	if (g_vec_pMeshesToDraw[target - 1]->friendlyName.find(",Stair:") == std::string::npos) {
	//		target--;
	//		//if (target == 0) {
	//		//	prevColor = g_vec_pMeshesToDraw[target]->RGBA;
	//		//	prevTarget = target;
	//		//	g_vec_pMeshesToDraw[target]->RGBA = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	//		//}
	//		//else {
	//		//	g_vec_pMeshesToDraw[prevTarget]->RGBA = prevColor;
	//		//	prevColor = g_vec_pMeshesToDraw[target]->RGBA;
	//		//	prevTarget = target;
	//		//	g_vec_pMeshesToDraw[target]->RGBA = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	//		//}
	//		//g_vec_pMeshesToDraw[target]->RGBA = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	//	}
	//	else {
	//		//std::cout << "target: " << target << std::endl;
	//		target--;
	//		break;
	//	}

	//}
	//if (target - 1 < 0) {
	//	target = g_vec_pMeshesToDraw.size() -1;
	//}
	//std::cout << "target: " << target << std::endl;
}

void cMesh::duplicateCurrentTarget(std::vector<cMesh*>& g_vec_pMeshesToDraw)
{
	cMesh* pMesh = new cMesh();
	*pMesh = *g_vec_pMeshesToDraw[target];
	pMesh->friendlyName = pMesh->friendlyName + "_DUPLICATE";
	pMesh->overallScale = g_vec_pMeshesToDraw[target]->overallScale;
	g_vec_pMeshesToDraw.push_back(pMesh);
	target = g_vec_pMeshesToDraw.size() - 1;
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
