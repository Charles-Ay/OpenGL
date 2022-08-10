#include "globalStuff.h"

float someValue = 0.0f;

double timerCounter = -1.0;

void UpdateEachFrame(double deltaTime)
{
	// Find the cow and change the transparency

	cMesh* pCow = ::g_findObjectByFriendlyName("Betsy");
	if (pCow)
	{
		// There's a valid pointer (i.e. NOT zero)

		someValue += 0.01f;
		pCow->RGBA.a = fabs(cos(someValue));
	}

	// Blend the whale's texture over time
	cMesh* pWhale = ::g_findObjectByFriendlyName("MobyDickJr");
	if (pWhale)
	{
		// Crazy colour is #2
		pWhale->textureRatio[0] = (cos(someValue) + 1.0) * 0.5f;
		pWhale->textureRatio[2] = 1.0 - pWhale->textureRatio[0];
		if (timerCounter < 0.0)
		{
			timerCounter = 0.0;
		}
		else
		{
			// Add the frame time to the timerCounter
			timerCounter += deltaTime;
		}
		// Has 5 seconds passed?
		if (timerCounter >= 5.0)
		{
			pWhale->textures[0] = "RocksOnABeach.bmp";
		}
		if (timerCounter >= 10.0)
		{
			pWhale->meshFileName = "assets/models/Dungeon_models/Walls/SM_Env_Dwarf_Wall_Archway_04.ply";
		}
		if (timerCounter >= 15.0)
		{
			pWhale->meshFileName = "assets/models/BlueWhale/BlueWhale_1000x_bigger.ply";
		}
	}//if ( pWhale )

	return;
}