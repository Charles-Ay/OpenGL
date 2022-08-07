
#include "cBasicTextureManager/cBasicTextureManager.h"
#include "cMesh.h"
#include "cLoadTexture.h"

#include <vector>
#include <iostream>
#include <string>

void loadAllTextures(std::vector<cMesh*>& g_vec_pMeshesToDraw, cBasicTextureManager*& pTheTexureManager)
{

    for (int i = 0; i < g_vec_pMeshesToDraw.size(); i++) {
		
    }
    for (cMesh* mesh : g_vec_pMeshesToDraw)
    {
        pTheTexureManager->SetBasePath(mesh->texturePath);
        for (int i = 0; i < mesh->usedTextures; i++) {
            if (!pTheTexureManager->Create2DTextureFromBMPFile(mesh->textures[i], true))
            {
                std::cout << "Error: Didn't load " << mesh->textures[i] << std::endl;
            }
        }
    }
}
