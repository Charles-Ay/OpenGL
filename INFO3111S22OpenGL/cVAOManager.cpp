#include "cVAOManager.h"

//#include "globalOpenGLStuff.h"
#include "globalStuff.h"

#include <fstream>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <vector>

#include <sstream>

sModelDrawInfo::sModelDrawInfo()
{
	this->VAO_ID = 0;

	this->VertexBufferID = 0;
	this->VertexBuffer_Start_Index = 0;
	this->numberOfVertices = 0;

	this->IndexBufferID = 0;
	this->IndexBuffer_Start_Index = 0;
	this->numberOfIndices = 0;
	this->numberOfTriangles = 0;

	// The "local" (i.e. "CPU side" temporary array)
	this->pVertices = 0;	// or NULL
	this->pIndices = 0;		// or NULL


	return;
}


bool cVAOManager::LoadModelIntoVAO(
	std::string fileName,
	sModelDrawInfo& drawInfo,
	unsigned int shaderProgramID)

{
	// Load the model from file
	// (We do this here, since if we can't load it, there's 
	//	no point in doing anything else, right?)

	drawInfo.meshName = fileName;

	//	if ( ! this->m_LoadTheModel( fileName, drawInfo ) )
	//	{
	//		this->m_AppendTextToLastError( "Didn't load model", true );
	//		return false;
	//	}
	ePLYFileType fileTypeDetected = ePLYFileType::UNKNOWN_FORMAT;
	if (!this->m_LoadTheModel_2(fileName, drawInfo, fileTypeDetected))
	{
		this->m_AppendTextToLastError("Didn't load model", true);
		return false;
	}
	// 
	// Model is loaded and the vertices and indices are in the drawInfo struct
	// 

	// Create a VAO (Vertex Array Object), which will 
	//	keep track of all the 'state' needed to draw 
	//	from this buffer...

	// Ask OpenGL for a new buffer ID...
	glGenVertexArrays(1, &(drawInfo.VAO_ID));
	// "Bind" this buffer:
	// - aka "make this the 'current' VAO buffer
	glBindVertexArray(drawInfo.VAO_ID);

	// Now ANY state that is related to vertex or index buffer
	//	and vertex attribute layout, is stored in the 'state' 
	//	of the VAO... 


	// NOTE: OpenGL error checks have been omitted for brevity
//	glGenBuffers(1, &vertex_buffer);
	glGenBuffers(1, &(drawInfo.VertexBufferID));

	//	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, drawInfo.VertexBufferID);
	// sVert vertices[3]
	glBufferData(GL_ARRAY_BUFFER,
		//				  sizeof(sVert) * drawInfo.numberOfVertices,	// ::g_NumberOfVertsToDraw,	// sizeof(vertices), 
		sizeof(sVert_n_rgba_uv) * drawInfo.numberOfVertices,	// ::g_NumberOfVertsToDraw,	// sizeof(vertices), 
		(GLvoid*)drawInfo.pVertices,							// pVertices,			//vertices, 
		GL_STATIC_DRAW);


	// Copy the index buffer into the video card, too
	// Create an index buffer.
	glGenBuffers(1, &(drawInfo.IndexBufferID));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawInfo.IndexBufferID);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER,			// Type: Index element array
		sizeof(unsigned int) * drawInfo.numberOfIndices,
		(GLvoid*)drawInfo.pIndices,
		GL_STATIC_DRAW);

	// Set the vertex attributes.

//	GLint vpos_location = glGetAttribLocation(shaderProgramID, "vPos");	// program
//	GLint vcol_location = glGetAttribLocation(shaderProgramID, "vCol");	// program;
//
//	// Set the vertex attributes for this shader
//	glEnableVertexAttribArray(vpos_location);	// vPos
//	glVertexAttribPointer( vpos_location, 3,		// vPos
//						   GL_FLOAT, GL_FALSE,
//						   sizeof(float) * 6, 
//						   ( void* )0);
//
//	glEnableVertexAttribArray(vcol_location);	// vCol
//	glVertexAttribPointer( vcol_location, 3,		// vCol
//						   GL_FLOAT, GL_FALSE,
//						   sizeof(float) * 6, 
//						   ( void* )( sizeof(float) * 3 ));

	// This is called the "vertex layout" and tells the shader
	// where it's supposed to find the vertex day
	GLint vPosition_location = glGetAttribLocation(shaderProgramID, "vPosition");	// program
	glEnableVertexAttribArray(vPosition_location);
	glVertexAttribPointer(vPosition_location,
		4,
		GL_FLOAT,
		GL_FALSE,
		sizeof(sVert_n_rgba_uv),		// How many bytes is the "stride" or size of each vertex
		(void*)offsetof(sVert_n_rgba_uv, x));

	GLint vRGBA_location = glGetAttribLocation(shaderProgramID, "vRGBA");	// program
	glEnableVertexAttribArray(vRGBA_location);
	glVertexAttribPointer(vRGBA_location,
		4,
		GL_FLOAT,
		GL_FALSE,
		sizeof(sVert_n_rgba_uv),		// How many bytes is the "stride" or size of each vertex
		(void*)offsetof(sVert_n_rgba_uv, r));

	GLint vNormal_location = glGetAttribLocation(shaderProgramID, "vNormal");	// program
	glEnableVertexAttribArray(vNormal_location);
	glVertexAttribPointer(vNormal_location,
		4,
		GL_FLOAT,
		GL_FALSE,
		sizeof(sVert_n_rgba_uv),		// How many bytes is the "stride" or size of each vertex
		(void*)offsetof(sVert_n_rgba_uv, nx));

	GLint vUV_x2_location = glGetAttribLocation(shaderProgramID, "vUV_x2");	// program
	glEnableVertexAttribArray(vUV_x2_location);
	glVertexAttribPointer(vUV_x2_location,
		4,
		GL_FLOAT,
		GL_FALSE,
		sizeof(sVert_n_rgba_uv),		// How many bytes is the "stride" or size of each vertex
		(void*)offsetof(sVert_n_rgba_uv, u1));

	// Now that all the parts are set up, set the VAO to zero
	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//	glDisableVertexAttribArray(vpos_location);
	//	glDisableVertexAttribArray(vcol_location);
	glDisableVertexAttribArray(vPosition_location);
	glDisableVertexAttribArray(vRGBA_location);
	glDisableVertexAttribArray(vNormal_location);
	glDisableVertexAttribArray(vUV_x2_location);


	// Store the draw information into the map
	this->m_map_ModelName_to_VAOID[drawInfo.meshName] = drawInfo;


	return true;
}


// We don't want to return an int, likely
bool cVAOManager::FindDrawInfoByModelName(
	std::string filename,
	sModelDrawInfo& drawInfo)
{
	std::map< std::string /*model name*/,
		sModelDrawInfo /* info needed to draw*/ >::iterator
		itDrawInfo = this->m_map_ModelName_to_VAOID.find(filename);

	// Find it? 
	if (itDrawInfo == this->m_map_ModelName_to_VAOID.end())
	{
		// Nope
		return false;
	}

	// Else we found the thing to draw
	// ...so 'return' that information
	drawInfo = itDrawInfo->second;
	return true;
}


bool cVAOManager::m_LoadTheModel(std::string fileName,
	sModelDrawInfo& drawInfo)
{
	// Open the file. 
	// Read until we hit the word "vertex"
	// Read until we hit the word "face"
	// Read until we hit the word "end_header"

	std::ifstream thePlyFile(fileName.c_str());
	if (!thePlyFile.is_open())
	{	// Something is wrong...
		std::stringstream ssError;
		ssError << "Can't open >" << fileName << "< file." << std::endl;
		this->m_AppendTextToLastError(ssError.str());
		return false;
	}

	std::string temp;
	while (thePlyFile >> temp)
	{
		if (temp == "vertex")
		{
			break;
		}
	};
	// read the number of vertices...
	thePlyFile >> drawInfo.numberOfVertices;	//::g_NumberOfVertices;

	while (thePlyFile >> temp)
	{
		if (temp == "face")
		{
			break;
		}
	};
	// read the number of triangles...
	thePlyFile >> drawInfo.numberOfTriangles;		// ::g_NumberOfTriangles;


	while (thePlyFile >> temp)
	{
		if (temp == "end_header")
		{
			break;
		}
	};

	// And now, we start reading vertices... Hazzah!

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
	// Load the vertices...
	for (unsigned int index = 0; index != drawInfo.numberOfVertices; // ::g_NumberOfVertices; 
		index++)
	{

		//		property float x
		//		property float y
		//		property float z		
		thePlyFile >> tempVert.pos.x >> tempVert.pos.y >> tempVert.pos.z;

		//		property float nx
		//		property float ny
		//		property float nz
		thePlyFile >> tempVert.normal.x >> tempVert.normal.y >> tempVert.normal.z;

		//		property uchar red
		//		property uchar green
		//		property uchar blue
		//		property uchar alpha
		thePlyFile >> tempVert.colourRGBA.r >> tempVert.colourRGBA.g >> tempVert.colourRGBA.b >> tempVert.colourRGBA.a;
		// The file has these as 0-255 (one byte) values, 
		//	but we want them as 0.0 - 1.0 values (for the shader)
		tempVert.colourRGBA.r /= 255.0f;
		tempVert.colourRGBA.g /= 255.0f;
		tempVert.colourRGBA.b /= 255.0f;
		tempVert.colourRGBA.a /= 255.0f;
		// 
		//		property float texture_u
		//		property float texture_v
				// Texture (UV or ST) coordinates are from 0.0 to 1.0 as well
		thePlyFile >> tempVert.textureCoords.x >> tempVert.textureCoords.y;


		// Add too... what? 
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
		thePlyFile >> discard						// the "3" at the start
			>> tempTriangle.vindex[0]
			>> tempTriangle.vindex[1]
			>> tempTriangle.vindex[2];

		//// Look up the vertex that matches the triangle index values.
		//tempTriangle.verts[0] = vecTempPlyVerts[ tempTriangle.vindex[0] ];
		//tempTriangle.verts[1] = vecTempPlyVerts[ tempTriangle.vindex[1] ];
		//tempTriangle.verts[2] = vecTempPlyVerts[ tempTriangle.vindex[2] ];

		vecTempPlyTriangles.push_back(tempTriangle);
	}//for ( unsigned int index...


	// NOW, we need to put them into the vertex array buffer that 
	//	will be passed to OpenGL. Why? 
	// Because we called glDrawArrays(), that's why. 

//	::g_NumberOfVertsToDraw = ::g_NumberOfTriangles * 3;	// 3 because "triangles"

	drawInfo.numberOfIndices = drawInfo.numberOfTriangles * 3;

	// sVert* pVertices = 0;
//	pVertices = new sVert[::g_NumberOfVertsToDraw];

	drawInfo.pIndices = new unsigned int[drawInfo.numberOfIndices];

	// Optional clear array to zero 
	//memset( drawInfo.pIndices, 0, sizeof(unsigned int) * drawInfo.numberOfIndices);

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

		//pVertices[ vertIndex + 0 ].x = curTri.verts[0].pos.x;
		//pVertices[ vertIndex + 0 ].y = curTri.verts[0].pos.y;
		//pVertices[ vertIndex + 0 ].z = curTri.verts[0].pos.z;
		//pVertices[ vertIndex + 0 ].r = curTri.verts[0].colour.x;
		//pVertices[ vertIndex + 0 ].g = curTri.verts[0].colour.y;
		//pVertices[ vertIndex + 0 ].b = curTri.verts[0].colour.z;
//
		//pVertices[ vertIndex + 1 ].x = curTri.verts[1].pos.x;
		//pVertices[ vertIndex + 1 ].y = curTri.verts[1].pos.y;
		//pVertices[ vertIndex + 1 ].z = curTri.verts[1].pos.z;
		//pVertices[ vertIndex + 1 ].r = curTri.verts[1].colour.x;
		//pVertices[ vertIndex + 1 ].g = curTri.verts[1].colour.y;
		//pVertices[ vertIndex + 1 ].b = curTri.verts[1].colour.z;
//
		//pVertices[ vertIndex + 2 ].x = curTri.verts[2].pos.x;
		//pVertices[ vertIndex + 2 ].y = curTri.verts[2].pos.y;
		//pVertices[ vertIndex + 2 ].z = curTri.verts[2].pos.z;
		//pVertices[ vertIndex + 2 ].r = curTri.verts[2].colour.x;
		//pVertices[ vertIndex + 2 ].g = curTri.verts[2].colour.y;
		//pVertices[ vertIndex + 2 ].b = curTri.verts[2].colour.z;
//
	}// for ( unsigned int triIndex = 0...

	this->CalculateExtents(drawInfo);

	return true;
}


std::string cVAOManager::getLastError(bool bAndClear /*=true*/)
{
	std::string theLastError = this->m_lastErrorString;

	if (bAndClear)
	{
		this->m_lastErrorString = "";
	}

	return theLastError;
}

void cVAOManager::m_AppendTextToLastError(std::string text, bool addNewLineBefore /*=true*/)
{
	std::stringstream ssError;
	ssError << this->m_lastErrorString;

	if (addNewLineBefore)
	{
		ssError << std::endl;
	}

	ssError << text;

	this->m_lastErrorString = ssError.str();

	return;

}

void cVAOManager::GetLoadedModelList(std::vector<std::string>& vecModelList)
{
	for (std::map< std::string, sModelDrawInfo>::iterator itModel = this->m_map_ModelName_to_VAOID.begin();
		itModel != this->m_map_ModelName_to_VAOID.end(); itModel++)
	{
		vecModelList.push_back(itModel->first);
	}
	return;
}
