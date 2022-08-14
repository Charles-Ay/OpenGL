
#include "cBasicTextureManager/cBasicTextureManager.h"
#include "cMesh.h"
#include "cLoadTexture.h"

#include <vector>
#include <iostream>
#include <string>

void loadAllTextures(std::vector<cMesh*>& g_vec_pMeshesToDraw, cBasicTextureManager*& pTheTexureManager)
{
    int loaded = 0;
    std::vector<std::string>loadedTextures;
    for (cMesh* mesh : g_vec_pMeshesToDraw)
    {
        pTheTexureManager->SetBasePath(mesh->texturePath);
        for (int i = 0; i < mesh->usedTextures; i++) {
            if (std::find(loadedTextures.begin(), loadedTextures.end(), mesh->textures[i]) == loadedTextures.end()) {
                if (!pTheTexureManager->Create2DTextureFromBMPFile(mesh->textures[i], true))
                {
                    std::cout << "Error: Didn't load texture for: " << mesh->textures[i] << std::endl;
                }
                else {
                    std::cout << "Loading texture for: " << mesh->friendlyName << std::endl;
                    ++loaded;
                    loadedTextures.push_back(mesh->textures[i]);
                }
            }
			

        }
    }
	
    std::cout << loaded << " textures loaded" << std::endl;
}
