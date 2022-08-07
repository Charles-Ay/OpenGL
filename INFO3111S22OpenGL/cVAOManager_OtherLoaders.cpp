#include "cVAOManager.h"

#include <fstream>
#include <sstream>



bool cVAOManager::m_LoadTheModel_2(std::string fileName,
								   sModelDrawInfo &drawInfo,
								   ePLYFileType &fileTypeDetected)
{
	// Figure out what kind of file this is
	std::string strError;
	fileTypeDetected = this->DetectPLYHeader(fileName, strError);
	if (fileTypeDetected == cVAOManager::ePLYFileType::UNKNOWN_FORMAT)
	{
		return false;
	}

	std::ifstream thePlyFile(fileName.c_str(), std::ios::binary);
	if (!thePlyFile.is_open())
	{
		return false;
	}

	thePlyFile.seekg(0, std::ios::end);
	// reutrns "std::ios::pos_type" type (which seems to be int in VC++)
	unsigned int fileSize = static_cast<unsigned int>(thePlyFile.tellg());
	// Return to start
	thePlyFile.seekg(0, std::ios::beg);

	char* pRawData = new char[fileSize];
	unsigned int curIndex = 0;				// Location in the array

	// read the file
	thePlyFile.read(pRawData, fileSize);
	thePlyFile.close();

	// *****************************************************************************
	// Process the header information
	cVAOManager::cStreamReader reader;

	std::string tempString;

	do 
	{
		tempString = reader.ASCIIReadNextString(pRawData, curIndex, fileSize);	
	} while (tempString != "vertex");

	// read the number of vertices...
	drawInfo.numberOfVertices = reader.ASCIIReadNextInt(pRawData, curIndex, fileSize);

	do
	{
		tempString = reader.ASCIIReadNextString(pRawData, curIndex, fileSize);
	} while (tempString != "face");

	// read the number of triangles...
	drawInfo.numberOfTriangles = reader.ASCIIReadNextInt(pRawData, curIndex, fileSize);

	do
	{
		tempString = reader.ASCIIReadNextString(pRawData, curIndex, fileSize);
	} while (tempString != "end_header");


	// This is set up to match the ply (3d model) file. 
	// NOT the shader. 
//		ply
//		format ascii 1.0
//		comment VCGLIB generated
//		element vertex 209466
//		property float x
//		property float y
//		property float z
//		property float nx
//		property float ny
//		property float nz
//		property uchar red
//		property uchar green
//		property uchar blue
//		property uchar alpha
//		property float texture_u
//		property float texture_v
//		element face 69451
//		property list uchar int vertex_indices
//		end_header
//		- 0.0378297 0.12794 0.00447467 0.1961509 0.9725953 - 0.1248331 255 255 255 255 0.373317 0.4743809
	struct sVertexPlyFileFormat
	{
		glm::vec3 pos;
		glm::vec3 normal;
		glm::vec4 colourRGBA;
		glm::vec2 textureCoords;
	};

	std::vector<sVertexPlyFileFormat> vecTempPlyVerts;

	sVertexPlyFileFormat tempVert;

	// Clear everything to a "safe" value
	tempVert.pos = glm::vec3(0.0f, 0.0f, 0.0f);
	tempVert.normal = glm::vec3(0.0f, 0.0f, 0.0f);
	// Setting this to 1.0 (all while) instead of 0.0 (all black)
	//	because a completely black object won't be seen at all. 
	tempVert.colourRGBA = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	tempVert.textureCoords = glm::vec2(0.0f, 0.0f);

	// Load the vertices...
	for (unsigned int index = 0; index != drawInfo.numberOfVertices; // ::g_NumberOfVertices; 
		 index++)
	{
		// They all start with xyz first...
		tempVert.pos.x = reader.ASCIIReadNextFloat(pRawData, curIndex, fileSize);
		tempVert.pos.y = reader.ASCIIReadNextFloat(pRawData, curIndex, fileSize);
		tempVert.pos.z = reader.ASCIIReadNextFloat(pRawData, curIndex, fileSize);

		// Depending on the type of header, read the rest of the vertex info

		switch (fileTypeDetected)
		{
//		case cVAOManager::ePLYFileType::XYZ_ONLY:
// Already dealth with above
//			break;
		case cVAOManager::ePLYFileType::XYZ_NORMAL:
			tempVert.normal.x = reader.ASCIIReadNextFloat(pRawData, curIndex, fileSize);
			tempVert.normal.y = reader.ASCIIReadNextFloat(pRawData, curIndex, fileSize);
			tempVert.normal.z = reader.ASCIIReadNextFloat(pRawData, curIndex, fileSize);
			break;
		case cVAOManager::ePLYFileType::XYZ_NORMAL_RGBA:
			tempVert.normal.x = reader.ASCIIReadNextFloat(pRawData, curIndex, fileSize);
			tempVert.normal.y = reader.ASCIIReadNextFloat(pRawData, curIndex, fileSize);
			tempVert.normal.z = reader.ASCIIReadNextFloat(pRawData, curIndex, fileSize);
			tempVert.colourRGBA.r = (float)reader.ASCIIReadNextInt(pRawData, curIndex, fileSize)/255.0f;
			tempVert.colourRGBA.g = (float)reader.ASCIIReadNextInt(pRawData, curIndex, fileSize)/255.0f;
			tempVert.colourRGBA.b = (float)reader.ASCIIReadNextInt(pRawData, curIndex, fileSize)/255.0f;
			tempVert.colourRGBA.a = (float)reader.ASCIIReadNextInt(pRawData, curIndex, fileSize)/255.0f;
			break;
		case cVAOManager::ePLYFileType::XYZ_RGBA:
			tempVert.colourRGBA.r = (float)reader.ASCIIReadNextInt(pRawData, curIndex, fileSize)/255.0f;
			tempVert.colourRGBA.g = (float)reader.ASCIIReadNextInt(pRawData, curIndex, fileSize)/255.0f;
			tempVert.colourRGBA.b = (float)reader.ASCIIReadNextInt(pRawData, curIndex, fileSize)/255.0f;
			tempVert.colourRGBA.a = (float)reader.ASCIIReadNextInt(pRawData, curIndex, fileSize)/255.0f;
			break;
		case cVAOManager::ePLYFileType::XYZ_UV:
			tempVert.textureCoords.x = reader.ASCIIReadNextFloat(pRawData, curIndex, fileSize);
			tempVert.textureCoords.y = reader.ASCIIReadNextFloat(pRawData, curIndex, fileSize);
			break;
		case cVAOManager::ePLYFileType::XYZ_NORMAL_UV:
			tempVert.normal.x = reader.ASCIIReadNextFloat(pRawData, curIndex, fileSize);
			tempVert.normal.y = reader.ASCIIReadNextFloat(pRawData, curIndex, fileSize);
			tempVert.normal.z = reader.ASCIIReadNextFloat(pRawData, curIndex, fileSize);
			tempVert.textureCoords.x = reader.ASCIIReadNextFloat(pRawData, curIndex, fileSize);
			tempVert.textureCoords.y = reader.ASCIIReadNextFloat(pRawData, curIndex, fileSize);
			break;
		case cVAOManager::ePLYFileType::XYZ_NORMAL_UV_RGBA:		// Blender
			tempVert.normal.x = reader.ASCIIReadNextFloat(pRawData, curIndex, fileSize);
			tempVert.normal.y = reader.ASCIIReadNextFloat(pRawData, curIndex, fileSize);
			tempVert.normal.z = reader.ASCIIReadNextFloat(pRawData, curIndex, fileSize);
			tempVert.textureCoords.x = reader.ASCIIReadNextFloat(pRawData, curIndex, fileSize);
			tempVert.textureCoords.y = reader.ASCIIReadNextFloat(pRawData, curIndex, fileSize);
			tempVert.colourRGBA.r = (float)reader.ASCIIReadNextInt(pRawData, curIndex, fileSize)/255.0f;
			tempVert.colourRGBA.g = (float)reader.ASCIIReadNextInt(pRawData, curIndex, fileSize)/255.0f;
			tempVert.colourRGBA.b = (float)reader.ASCIIReadNextInt(pRawData, curIndex, fileSize)/255.0f;
			tempVert.colourRGBA.a = (float)reader.ASCIIReadNextInt(pRawData, curIndex, fileSize)/255.0f;
			break;
		case cVAOManager::ePLYFileType::XYZ_NORMAL_RGBA_UV:		// MeshLab (VCGLIB)
			tempVert.normal.x = reader.ASCIIReadNextFloat(pRawData, curIndex, fileSize);
			tempVert.normal.y = reader.ASCIIReadNextFloat(pRawData, curIndex, fileSize);
			tempVert.normal.z = reader.ASCIIReadNextFloat(pRawData, curIndex, fileSize);
			tempVert.colourRGBA.r = (float)reader.ASCIIReadNextInt(pRawData, curIndex, fileSize)/255.0f;
			tempVert.colourRGBA.g = (float)reader.ASCIIReadNextInt(pRawData, curIndex, fileSize)/255.0f;
			tempVert.colourRGBA.b = (float)reader.ASCIIReadNextInt(pRawData, curIndex, fileSize)/255.0f;
			tempVert.colourRGBA.a = (float)reader.ASCIIReadNextInt(pRawData, curIndex, fileSize)/255.0f;
			tempVert.textureCoords.x = reader.ASCIIReadNextFloat(pRawData, curIndex, fileSize);
			tempVert.textureCoords.y = reader.ASCIIReadNextFloat(pRawData, curIndex, fileSize);
			break;
		case cVAOManager::ePLYFileType::XYZ_UV_RGBA:		// Blender
			tempVert.textureCoords.x = reader.ASCIIReadNextFloat(pRawData, curIndex, fileSize);
			tempVert.textureCoords.y = reader.ASCIIReadNextFloat(pRawData, curIndex, fileSize);
			tempVert.colourRGBA.r = (float)reader.ASCIIReadNextInt(pRawData, curIndex, fileSize)/255.0f;
			tempVert.colourRGBA.g = (float)reader.ASCIIReadNextInt(pRawData, curIndex, fileSize)/255.0f;
			tempVert.colourRGBA.b = (float)reader.ASCIIReadNextInt(pRawData, curIndex, fileSize)/255.0f;
			tempVert.colourRGBA.a = (float)reader.ASCIIReadNextInt(pRawData, curIndex, fileSize)/255.0f;
			break;
		case cVAOManager::ePLYFileType::XYZ_RGBA_UV:		// MeshLab (VCGLIB)
			tempVert.colourRGBA.r = (float)reader.ASCIIReadNextInt(pRawData, curIndex, fileSize)/255.0f;
			tempVert.colourRGBA.g = (float)reader.ASCIIReadNextInt(pRawData, curIndex, fileSize)/255.0f;
			tempVert.colourRGBA.b = (float)reader.ASCIIReadNextInt(pRawData, curIndex, fileSize)/255.0f;
			tempVert.colourRGBA.a = (float)reader.ASCIIReadNextInt(pRawData, curIndex, fileSize)/255.0f;
			tempVert.textureCoords.x = reader.ASCIIReadNextFloat(pRawData, curIndex, fileSize);
			tempVert.textureCoords.y = reader.ASCIIReadNextFloat(pRawData, curIndex, fileSize);
			break;
		default:
			// This "shouldn't" happen, but who knows?
			return false;
			break;
		}//switch (plyFileType)

		vecTempPlyVerts.push_back(tempVert);
	}

	// Create a local vertex array
	// Note: The format the file (ply) is DIFFERENT from this array:
	// - sVertPly was made to match the file format
	// - sVert was made to match the shader vertex attrib format

//	drawInfo.pVertices = new sVert[drawInfo.numberOfVertices];
	drawInfo.pVertices = new sVert_n_rgba_uv[drawInfo.numberOfVertices];

	// Optional clear array to zero 
	//memset( drawInfo.pVertices, 0, sizeof(sVert) * drawInfo.numberOfVertices);

	for (unsigned int index = 0; index != drawInfo.numberOfVertices; index++)
	{
		drawInfo.pVertices[index].x = vecTempPlyVerts[index].pos.x;
		drawInfo.pVertices[index].y = vecTempPlyVerts[index].pos.y;
		drawInfo.pVertices[index].z = vecTempPlyVerts[index].pos.z;
		drawInfo.pVertices[index].w = 1.0f;		// this isn't in the ply file

		drawInfo.pVertices[index].r = vecTempPlyVerts[index].colourRGBA.r;
		drawInfo.pVertices[index].g = vecTempPlyVerts[index].colourRGBA.g;
		drawInfo.pVertices[index].b = vecTempPlyVerts[index].colourRGBA.b;
		drawInfo.pVertices[index].a = vecTempPlyVerts[index].colourRGBA.a;

		// Also normals...
		drawInfo.pVertices[index].nx = vecTempPlyVerts[index].normal.x;
		drawInfo.pVertices[index].ny = vecTempPlyVerts[index].normal.y;
		drawInfo.pVertices[index].nz = vecTempPlyVerts[index].normal.z;
		drawInfo.pVertices[index].nw = 1.0f;	// This isn't in the ply file

		// Also UV (texture) coordinates
		drawInfo.pVertices[index].u1 = vecTempPlyVerts[index].textureCoords.x;
		drawInfo.pVertices[index].v1 = vecTempPlyVerts[index].textureCoords.y;
		drawInfo.pVertices[index].u2 = 0.0f;		// not in the PLY file
		drawInfo.pVertices[index].v2 = 0.0f;		// Not in the PLY file


	}// for ( unsigned int index...


	struct sTriPly
	{
		unsigned int vindex[3];		// the 3 indices
		sVertexPlyFileFormat verts[3];			// The actual vertices
	};

	std::vector<sTriPly> vecTempPlyTriangles;
	sTriPly tempTriangle;
	unsigned int discard = 0;
	for (unsigned int index = 0; index != drawInfo.numberOfTriangles;	// ::g_NumberOfTriangles; 
		 index++)
	{
		// 3 5622 5601 5640
		discard = reader.ASCIIReadNextInt(pRawData, curIndex, fileSize);
		tempTriangle.vindex[0] = reader.ASCIIReadNextInt(pRawData, curIndex, fileSize);
		tempTriangle.vindex[1] = reader.ASCIIReadNextInt(pRawData, curIndex, fileSize);
		tempTriangle.vindex[2] = reader.ASCIIReadNextInt(pRawData, curIndex, fileSize);

		vecTempPlyTriangles.push_back(tempTriangle);
	}//for ( unsigned int index...

	drawInfo.numberOfIndices = drawInfo.numberOfTriangles * 3;

	drawInfo.pIndices = new unsigned int[drawInfo.numberOfIndices];

	// Allocate on the HEAP, so infinite size... 
	// delete pVertices			/// error!
	// delete [] pVertices		/// correct!!
	unsigned int indexBufferIndex = 0;
	for (unsigned int triIndex = 0;
		 triIndex != drawInfo.numberOfTriangles;		// ::g_NumberOfTriangles; 
		 triIndex++, indexBufferIndex += 3)
	{
		sTriPly& curTri = vecTempPlyTriangles[triIndex];

		// This is taking an array of triangles vertex indices 
		// (which is a 2D array of indices, really) and converting
		//  it into a 1D array of indices...)
		drawInfo.pIndices[indexBufferIndex + 0] = curTri.vindex[0];
		drawInfo.pIndices[indexBufferIndex + 1] = curTri.vindex[1];
		drawInfo.pIndices[indexBufferIndex + 2] = curTri.vindex[2];

	}// for ( unsigned int triIndex = 0...

	this->CalculateExtents(drawInfo);

	return true;
}

// This is used by the m_DetectPLYHeader to compare tokens it's read
bool cVAOManager::m_DoTokenListsMatch( const std::vector<std::string> &vecTokens, const std::vector<std::string>& compareArray )
{
	if ( vecTokens.size() != compareArray.size())
	{	// Not the same length
		return false;
	}

	for ( unsigned int index = 0; index != compareArray.size(); index++ )
	{
		if ( vecTokens[index] != compareArray[index] )
		{	
			// This token doesn't match
			return false;
		}
	}

	// Didn't find any mis-matches
	return true;
}

cVAOManager::ePLYFileType cVAOManager::DetectPLYHeader(std::string fileName, std::string& sError)
{
	cVAOManager::ePLYFileType fileTypeDetected;
	if ( ! this->m_DetectPLYHeader(fileName, fileTypeDetected, sError) )
	{
		return cVAOManager::UNKNOWN_FORMAT;
	}

	return fileTypeDetected;
}


bool cVAOManager::m_DetectPLYHeader(std::string fileName,
									ePLYFileType& fileTypeDetected,
									std::string &sError)
{
	// This is a quick-and-dirty header detector
	// Here's how it works:
	// 1. Scan until the word "vertex" and read the number of verts (and discard)
	// 2. Read the remaining tokens (after "property float"
	//		Place this into a vector for later
	// 3. Stop reading when we get to "element"

	//		ply
	//		format ascii 1.0
	//		comment VCGLIB generated
	//		element vertex 209466			<-- Scan to this
	//		property float x
	//		property float y
	//		property float z
	//		property float nx
	//		property float ny
	//		property float nz
	//		property uchar red
	//		property uchar green
	//		property uchar blue
	//		property uchar alpha
	//		property float texture_u
	//		property float texture_v
	//		element face 69451				<-- End of scan
	//		property list uchar int vertex_indices
	//		end_header
	//		- 0.0378297 0.12794 0.00447467 0.1961509 0.9725953 - 0.1248331 255 255 255 255 0.373317 0.4743809

	std::ifstream thePlyFile(fileName.c_str());
	if (!thePlyFile.is_open())
	{	// Something is wrong...
		std::stringstream ssError;
		ssError << "Can't open >" << fileName << "< file." << std::endl;
		this->m_AppendTextToLastError(ssError.str());
		return false;
	}

	std::string tempToken;
	while (thePlyFile >> tempToken)
	{
		if (tempToken == "vertex")
		{
			break;
		}
	};

	// read the number of vertices...
	unsigned int numberVerts = 0;
	thePlyFile >> numberVerts;	


	std::vector<std::string> vecProperties;
	while (thePlyFile >> tempToken)
	{
		if (tempToken == "element")
		{	// Done scanning 
			break;
		}
		else
		{
			//	property float x
			if (tempToken == "property")
			{
				// Consume the property type
				thePlyFile >> tempToken;		// Discard (float, char, etc.)
				// Read the property name
				thePlyFile >> tempToken;
				vecProperties.push_back(tempToken);	// x,y,n,u, etc.
			}
			else
			{
				// Something very unexpected happened
				thePlyFile.close();
				sError = "Read an unexpected token" + tempToken;
				return false;
			}
		}//if (tempToken == "element")
	}
	
	// Here's the various formats that both blender and MeshLab output when 
	// saving as PLY ("VCGLIB" is the DLL MeshLab uses):
	//
	//  XYZ_ONLY:
	//   www.blender.org        comment VCGLIB generated
	//   element vertex 1234    element vertex 1234
	//   property float x       property float x
	//   property float y       property float y
	//   property float z       property float z
	//   element face 4321      element face 4321
	//
	{//	XYZ_ONLY
		const std::vector<std::string> XYZ_ONLY_CompareArray = { "x", "y", "z" };
		if (this->m_DoTokenListsMatch(vecProperties, XYZ_ONLY_CompareArray))
		{
			fileTypeDetected = cVAOManager::XYZ_ONLY;
			thePlyFile.close();
			return true;
		}
	}//	XYZ_ONLY
	
	// XYZ_NORMAL:
	//   www.blender.org        comment VCGLIB generated
	//   element vertex 1234    element vertex 1234
	//   property float x       property float x
	//   property float y       property float y
	//   property float z       property float z
	//   property float nx      property float nx
	//   property float ny      property float ny
	//   property float nz      property float nz
	//   element face 4321      element face 4321
	//
	{//	XYZ_NORMAL
		const std::vector<std::string> XYZ_NORMAL_CompareArray = { "x", "y", "z", "nx", "ny", "nz" };
		if (this->m_DoTokenListsMatch(vecProperties, XYZ_NORMAL_CompareArray))
		{
			fileTypeDetected = cVAOManager::XYZ_NORMAL;
			thePlyFile.close();
			return true;
		}
	}//	XYZ_NORMAL

	//  XYZ_NORMAL_RGBA:
	//   www.blender.org        comment VCGLIB generated
	//   element vertex 1234    element vertex 1234
	//   property float x       property float x
	//   property float y       property float y
	//   property float z       property float z
	//   property float nx      property float nx
	//   property float ny      property float ny
	//   property float nz      property float nz
	//   property uchar red     property uchar red
	//   property uchar green   property uchar green
	//   property uchar blue    property uchar blue
	//   property uchar alpha   property uchar alpha
	//   element face 4321      element face 4321
	//
	{//	XYZ_NORMAL_RGBA
		const std::vector<std::string> XYZ_NORMAL_RGBA_CompareArray = { "x", "y", "z", "nx", "ny", "nz", "red", "green", "blue", "alpha" };
		if (this->m_DoTokenListsMatch(vecProperties, XYZ_NORMAL_RGBA_CompareArray))
		{
			fileTypeDetected = cVAOManager::XYZ_NORMAL_RGBA;
			thePlyFile.close();
			return true;
		}
	}//	XYZ_NORMAL_RGBA
	
	//  XYZ_RGBA:
	//   www.blender.org        comment VCGLIB generated
	//   element vertex 1234    element vertex 1234
	//   property float x       property float x
	//   property float y       property float y
	//   property float z       property float z
	//   property uchar red     property uchar red
	//   property uchar green   property uchar green
	//   property uchar blue    property uchar blue
	//   property uchar alpha   property uchar alpha
	//   element face 4321      element face 4321
	//
	{//	XYZ_RGBA
		const std::vector<std::string> XYZ_RGBA_CompareArray = { "x", "y", "z", "red", "green", "blue", "alpha" };
		if (this->m_DoTokenListsMatch(vecProperties, XYZ_RGBA_CompareArray))
		{
			fileTypeDetected = cVAOManager::XYZ_RGBA;
			thePlyFile.close();
			return true;
		}
	}//	XYZ_RGBA

	// The texture coordinates could be any one of these:
	// s, t 
	// u, v
	// texture_s, texture_t
	// texture_u, texture_v
	//
	// So we're going to check for all the combinations

	//  XYZ_UV:
	//   www.blender.org        comment VCGLIB generated
	//   element vertex 1234    element vertex 1234
	//   property float x       property float x
	//   property float y       property float y
	//   property float z       property float z
	//   property float s       property float texture_u
	//   property float t       property float texture_v
	//   element face 4321      element face 4321
	//
	{//	XYZ_UV
		const std::vector<std::string> XYZ_UV_CompareArray_blender_1 = { "x", "y", "z", "s", "t" };
		if (this->m_DoTokenListsMatch(vecProperties, XYZ_UV_CompareArray_blender_1))
		{
			fileTypeDetected = cVAOManager::XYZ_UV;
			thePlyFile.close();
			return true;
		}
		const std::vector<std::string> XYZ_UV_CompareArray_blender_2 = { "x", "y", "z", "texture_s", "texture_t" };
		if (this->m_DoTokenListsMatch(vecProperties, XYZ_UV_CompareArray_blender_2))
		{
			fileTypeDetected = cVAOManager::XYZ_UV;
			thePlyFile.close();
			return true;
		}
		const std::vector<std::string> XYZ_UV_CompareArray_meshlab_1 = { "x", "y", "z", "u", "v" };
		if (this->m_DoTokenListsMatch(vecProperties, XYZ_UV_CompareArray_meshlab_1))
		{
			fileTypeDetected = cVAOManager::XYZ_UV;
			thePlyFile.close();
			return true;
		}
		const std::vector<std::string> XYZ_UV_CompareArray_meshlab_2 = { "x", "y", "z", "texture_u", "texture_v" };
		if (this->m_DoTokenListsMatch(vecProperties, XYZ_UV_CompareArray_meshlab_2))
		{
			fileTypeDetected = cVAOManager::XYZ_UV;
			thePlyFile.close();
			return true;
		}
	}//	XYZ_UV
	
	//  XYZ_NORMAL_UV:          
	//   www.blender.org         comment VCGLIB generated
	//   element vertex 1234     element vertex 1234
	//   property float x        property float x
	//   property float y        property float y
	//   property float z        property float z
	//   property float nx       property float nx
	//   property float ny       property float ny
	//   property float nz       property float nz
	//   property float s        property float texture_u
	//   property float t        property float texture_v
	//   element face 4321       element face 4321
	//
	{//	XYZ_NORMAL_UV
		const std::vector<std::string> XYZ_NORMAL_UV_CompareArray_blender_1 = { "x", "y", "z", "nx", "ny", "nz", "s", "t" };
		if (this->m_DoTokenListsMatch(vecProperties, XYZ_NORMAL_UV_CompareArray_blender_1))
		{
			fileTypeDetected = cVAOManager::XYZ_NORMAL_UV;
			thePlyFile.close();
			return true;
		}
		const std::vector<std::string> XYZ_NORMAL_UV_CompareArray_blender_2 = { "x", "y", "z", "nx", "ny", "nz", "texture_s", "texture_t" };
		if (this->m_DoTokenListsMatch(vecProperties, XYZ_NORMAL_UV_CompareArray_blender_2))
		{
			fileTypeDetected = cVAOManager::XYZ_NORMAL_UV;
			thePlyFile.close();
			return true;
		}
		const std::vector<std::string> XYZ_NORMAL_UV_CompareArray_meshlab_1 = { "x", "y", "z", "nx", "ny", "nz", "u", "v" };
		if (this->m_DoTokenListsMatch(vecProperties, XYZ_NORMAL_UV_CompareArray_meshlab_1))
		{
			fileTypeDetected = cVAOManager::XYZ_NORMAL_UV;
			thePlyFile.close();
			return true;
		}
		const std::vector<std::string> XYZ_NORMAL_UV_CompareArray_meshlab_2 = { "x", "y", "z", "nx", "ny", "nz", "texture_u", "texture_v" };
		if (this->m_DoTokenListsMatch(vecProperties, XYZ_NORMAL_UV_CompareArray_meshlab_2))
		{
			fileTypeDetected = cVAOManager::XYZ_NORMAL_UV;
			thePlyFile.close();
			return true;
		}
	}//	XYZ_NORMAL_UV
	
	
	// These ones have the texture coordinates in different locations:
	//
	//  XYZ_NORMAL_UV_RGBA:     XYZ_NORMAL_RGBA_UV:
	//   www.blender.org         comment VCGLIB generated
	//   element vertex 1234     element vertex 1234
	//   property float x        property float x
	//   property float y        property float y
	//   property float z        property float z
	//   property float nx       property float nx
	//   property float ny       property float ny
	//   property float nz       property float nz
	//   property float s        property uchar red
	//   property float t        property uchar green
	//   property uchar red      property uchar blue
	//   property uchar green    property uchar alpha
	//   property uchar blue     property float texture_u
	//   property uchar alpha    property float texture_v
	//   element face 4321       element face 4321
	//
	{//	XYZ_NORMAL_UV_RGBA
		const std::vector<std::string> XYZ_NORMAL_UV_RGBA_CompareArray_blender_1 = { "x", "y", "z", "nx", "ny", "nz", "s", "t", "red", "green", "blue", "alpha" };
		if (this->m_DoTokenListsMatch(vecProperties, XYZ_NORMAL_UV_RGBA_CompareArray_blender_1))
		{
			fileTypeDetected = cVAOManager::XYZ_NORMAL_UV_RGBA;
			thePlyFile.close();
			return true;
		}
		const std::vector<std::string> XYZ_NORMAL_UV_RGBA_CompareArray_blender_2 = { "x", "y", "z", "nx", "ny", "nz", "texture_s", "texture_t", "red", "green", "blue", "alpha" };
		if (this->m_DoTokenListsMatch(vecProperties, XYZ_NORMAL_UV_RGBA_CompareArray_blender_2))
		{
			fileTypeDetected = cVAOManager::XYZ_NORMAL_UV_RGBA;
			thePlyFile.close();
			return true;
		}
		const std::vector<std::string> XYZ_NORMAL_UV_RGBA_CompareArray_meshlab_1 = { "x", "y", "z", "nx", "ny", "nz", "u", "v", "red", "green", "blue", "alpha" };
		if (this->m_DoTokenListsMatch(vecProperties, XYZ_NORMAL_UV_RGBA_CompareArray_meshlab_1))
		{
			fileTypeDetected = cVAOManager::XYZ_NORMAL_UV_RGBA;
			thePlyFile.close();
			return true;
		}
		const std::vector<std::string> XYZ_NORMAL_UV_RGBA_CompareArray_meshlab_2 = { "x", "y", "z", "nx", "ny", "nz", "texture_u", "texture_v", "red", "green", "blue", "alpha" };
		if (this->m_DoTokenListsMatch(vecProperties, XYZ_NORMAL_UV_RGBA_CompareArray_meshlab_2))
		{
			fileTypeDetected = cVAOManager::XYZ_NORMAL_UV_RGBA;
			thePlyFile.close();
			return true;
		}
	}//	XYZ_NORMAL_UV_RGBA
	{// XYZ_NORMAL_RGBA_UV
		const std::vector<std::string> XYZ_NORMAL_RGBA_UV_CompareArray_blender_1 = { "x", "y", "z", "nx", "ny", "nz", "red", "green", "blue", "alpha", "s", "t" };
		if (this->m_DoTokenListsMatch(vecProperties, XYZ_NORMAL_RGBA_UV_CompareArray_blender_1))
		{
			fileTypeDetected = cVAOManager::XYZ_NORMAL_RGBA_UV;
			thePlyFile.close();
			return true;
		}
		const std::vector<std::string> XYZ_NORMAL_RGBA_UV_CompareArray_blender_2 = { "x", "y", "z", "nx", "ny", "nz", "red", "green", "blue", "alpha", "texture_s", "texture_t" };
		if (this->m_DoTokenListsMatch(vecProperties, XYZ_NORMAL_RGBA_UV_CompareArray_blender_2))
		{
			fileTypeDetected = cVAOManager::XYZ_NORMAL_RGBA_UV;
			thePlyFile.close();
			return true;
		}
		const std::vector<std::string> XYZ_NORMAL_RGBA_UV_CompareArray_meshlab_1 = { "x", "y", "z", "nx", "ny", "nz", "red", "green", "blue", "alpha", "u", "v" };
		if (this->m_DoTokenListsMatch(vecProperties, XYZ_NORMAL_RGBA_UV_CompareArray_meshlab_1))
		{
			fileTypeDetected = cVAOManager::XYZ_NORMAL_RGBA_UV;
			thePlyFile.close();
			return true;
		}
		const std::vector<std::string> XYZ_NORMAL_RGBA_UV_CompareArray_meshlab_2 = { "x", "y", "z", "nx", "ny", "nz", "red", "green", "blue", "alpha", "texture_u", "texture_v" };
		if (this->m_DoTokenListsMatch(vecProperties, XYZ_NORMAL_RGBA_UV_CompareArray_meshlab_2))
		{
			fileTypeDetected = cVAOManager::XYZ_NORMAL_RGBA_UV;
			thePlyFile.close();
			return true;
		}
	}// XYZ_NORMAL_RGBA_UV


	//  XYZ_UV_RGBA:            XYZ_RGBA_UV:
	//   www.blender.org         comment VCGLIB generated
	//   element vertex 1234     element vertex 1234
	//   property float x        property float x
	//   property float y        property float y
	//   property float z        property float z
	//   property float s        property uchar red
	//   property float t        property uchar green
	//   property uchar red      property uchar blue
	//   property uchar green    property uchar alpha
	//   property uchar blue     property float texture_u
	//   property uchar alpha    property float texture_v
	//   element face 4321       element face 4321
	//
	{//	XYZ_UV_RGBA
		const std::vector<std::string> XYZ_UV_RGBA_CompareArray_blender_1 = { "x", "y", "z", "s", "t", "red", "green", "blue", "alpha" };
		if (this->m_DoTokenListsMatch(vecProperties, XYZ_UV_RGBA_CompareArray_blender_1))
		{
			fileTypeDetected = cVAOManager::XYZ_UV_RGBA;
			thePlyFile.close();
			return true;
		}
		const std::vector<std::string> XYZ_UV_RGBA_CompareArray_blender_2 = { "x", "y", "z", "texture_s", "texture_t", "red", "green", "blue", "alpha" };
		if (this->m_DoTokenListsMatch(vecProperties, XYZ_UV_RGBA_CompareArray_blender_2))
		{
			fileTypeDetected = cVAOManager::XYZ_UV_RGBA;
			thePlyFile.close();
			return true;
		}
		const std::vector<std::string> XYZ_UV_RGBA_CompareArray_meshlab_1 = { "x", "y", "z", "u", "v", "red", "green", "blue", "alpha" };
		if (this->m_DoTokenListsMatch(vecProperties, XYZ_UV_RGBA_CompareArray_meshlab_1))
		{
			fileTypeDetected = cVAOManager::XYZ_UV_RGBA;
			thePlyFile.close();
			return true;
		}
		const std::vector<std::string> XYZ_UV_RGBA_CompareArray_meshlab_2 = { "x", "y", "z", "texture_u", "texture_v", "red", "green", "blue", "alpha" };
		if (this->m_DoTokenListsMatch(vecProperties, XYZ_UV_RGBA_CompareArray_meshlab_2))
		{
			fileTypeDetected = cVAOManager::XYZ_UV_RGBA;
			thePlyFile.close();
			return true;
		}
	}//	XYZ_UV_RGBA
	{// XYZ_RGBA_UV
		const std::vector<std::string> XYZ_RGBA_UV_CompareArray_blender_1 = { "x", "y", "z", "red", "green", "blue", "alpha", "s", "t" };
		if (this->m_DoTokenListsMatch(vecProperties, XYZ_RGBA_UV_CompareArray_blender_1))
		{
			fileTypeDetected = cVAOManager::XYZ_RGBA_UV;
			thePlyFile.close();
			return true;
		}
		const std::vector<std::string> XYZ_RGBA_UV_CompareArray_blender_2 = { "x", "y", "z", "red", "green", "blue", "alpha", "texture_s", "texture_t" };
		if (this->m_DoTokenListsMatch(vecProperties, XYZ_RGBA_UV_CompareArray_blender_2))
		{
			fileTypeDetected = cVAOManager::XYZ_RGBA_UV;
			thePlyFile.close();
			return true;
		}
		const std::vector<std::string> XYZ_RGBA_UV_CompareArray_meshlab_1 = { "x", "y", "z", "red", "green", "blue", "alpha", "u", "v" };
		if (this->m_DoTokenListsMatch(vecProperties, XYZ_RGBA_UV_CompareArray_meshlab_1))
		{
			fileTypeDetected = cVAOManager::XYZ_RGBA_UV;
			thePlyFile.close();
			return true;
		}
		const std::vector<std::string> XYZ_RGBA_UV_CompareArray_meshlab_2 = { "x", "y", "z", "red", "green", "blue", "alpha", "texture_u", "texture_v" };
		if (this->m_DoTokenListsMatch(vecProperties, XYZ_RGBA_UV_CompareArray_meshlab_2))
		{
			fileTypeDetected = cVAOManager::XYZ_RGBA_UV;
			thePlyFile.close();
			return true;
		}
	}// XYZ_RGBA_UV


	 // Else... who knows what format this is??
	//	UNKNOWN_FORMAT	
	fileTypeDetected = cVAOManager::UNKNOWN_FORMAT;
	sError = "Unknown file format detected.";

	thePlyFile.close();

	return false;
}

std::string cVAOManager::TranslateHeaderTypeString(cVAOManager::ePLYFileType plyFileType)
{
	switch (plyFileType)
	{
		case cVAOManager::ePLYFileType::XYZ_ONLY:
			return "XYZ_ONLY";
			break;
		case cVAOManager::ePLYFileType::XYZ_NORMAL:
			return "XYZ_NORMAL";
			break;
		case cVAOManager::ePLYFileType::XYZ_NORMAL_RGBA:
			return "XYZ_NORMAL_RGBA";
			break;
		case cVAOManager::ePLYFileType::XYZ_RGBA:
			return "XYZ_RGBA";
			break;
		case cVAOManager::ePLYFileType::XYZ_UV:
			return "XYZ_UV";
			break;
		case cVAOManager::ePLYFileType::XYZ_NORMAL_UV:
			return "XYZ_NORMAL_UV";
			break;
		case cVAOManager::ePLYFileType::XYZ_NORMAL_UV_RGBA:		// Blender
			return "XYZ_NORMAL_UV_RGBA (Blender)";
			break;
		case cVAOManager::ePLYFileType::XYZ_NORMAL_RGBA_UV:		// MeshLab (VCGLIB)
			return "XYZ_NORMAL_RGBA_UV (MeshLab-VCGLIB)";
			break;
		case cVAOManager::ePLYFileType::XYZ_UV_RGBA:		// Blender
			return "XYZ_UV_RGBA (Blender)";
			break;
		case cVAOManager::ePLYFileType::XYZ_RGBA_UV:		// MeshLab (VCGLIB)
			return "XYZ_RGBA_UV (MeshLab-VCGLIB)";
			break;
		default:
			return "UNKNOWN_FORMAT";
			break;
	}//switch (plyFileType)

	// "shouldn't" get here...
	return "UNKNOWN_FORMAT";
}





//static 
const float cVAOManager::cStreamReader::DEFAULTROUNDSMALLFLOATTOZEROVALUE = FLT_MIN;

cVAOManager::cStreamReader::cStreamReader()
{
	this->m_bRoundSmallFloatToZeroFlag = true;
	this->m_roundToZeroValue = cVAOManager::cStreamReader::DEFAULTROUNDSMALLFLOATTOZEROVALUE;
	return;
}

// Added to deal with REALLY small float values screwing up rendering
void cVAOManager::cStreamReader::SetMinFloatRoundToZero(float minRoundToZero)
{
	this->m_roundToZeroValue = minRoundToZero;
	return;
}

float cVAOManager::cStreamReader::GetMinFloatRoundToZeroValue(void)
{
	return this->m_roundToZeroValue;
}

void cVAOManager::cStreamReader::SetRoundTinyFloatsToZeroOnLoadFlag(bool bNewRoundToZeroFlag)
{
	this->m_bRoundSmallFloatToZeroFlag = bNewRoundToZeroFlag;
	return;
}

bool cVAOManager::cStreamReader::GetRoundTinyFloatsToZeroOnLoadFlag(void)
{
	return this->m_bRoundSmallFloatToZeroFlag;
}


// Reads utill the next 
std::string cVAOManager::cStreamReader::ASCIIReadNextString(char* pData, unsigned int& curIndex, const unsigned int& arraySize)
{
	std::string returnString; char curChar = 0;
	// Gone to far in main array?
	while (curIndex < arraySize)
	{
		curChar = static_cast<char>(pData[curIndex]);
		if (isspace(curChar))
		{
			curIndex++;
			// There could be additional whitespace (like CR + LF or something)
			// Keep reading white space until we come to a non-whitespace character
			while ((curIndex < arraySize) && isspace(pData[curIndex]))
			{
				// "consume" this whitespace character
				curIndex++;
			}
			// Either we're off the end of the array (perhaps that's bad)...
			// ...or we've hit the first non-white space
			return returnString;
		}
		returnString += curChar;
		curIndex++;
	};
	return returnString;
}

int cVAOManager::cStreamReader::ASCIIReadNextInt(char* pData, unsigned int& curIndex, const unsigned int& arraySize)
{
	std::string tempString = this->ASCIIReadNextString(pData, curIndex, arraySize);
	int returnInt = atoi(tempString.c_str());
	return returnInt;
}

float cVAOManager::cStreamReader::ASCIIReadNextFloat(char* pData, unsigned int& curIndex, const unsigned int& arraySize)
{
	std::string tempString = this->ASCIIReadNextString(pData, curIndex, arraySize);
	// Has an "e" in there?
	//bool bSexyBreakpoint = false;
	//if ( tempString.find('e') != std::string::npos )
	//{
	//	bSexyBreakpoint = true;
	//}
	float returnFloat = static_cast<float>(atof(tempString.c_str()));
	if (this->m_bRoundSmallFloatToZeroFlag)
	{
		if (fabs(returnFloat) < this->m_roundToZeroValue)
		{
			returnFloat = 0.0f;
		}
	}
	return returnFloat;
}
