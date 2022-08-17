// This is from here: https://www.glfw.org/docs/latest/quick.html

#include "globalStuff.h"
//#include <glad/glad.h>
//#define GLFW_INCLUDE_NONE
//#include <GLFW/glfw3.h>

//#include "linmath.h"
#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> 
// glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <sstream>  // String Stream
#include <fstream>

#include "cShaderManager.h"
#include "cVAOManager.h"
#include "cLightManager.h"
#include "cBasicTextureManager/cBasicTextureManager.h"
#include "cLoadTexture.h"


void PlaceAModelInARandomLocation(std::string modelName, float minScale, float maxScale);

void CopyLightInformationToShader(cLightManager* pTheLightManager, GLuint shaderID);

// These are defined in the InputHandlerfunctions.cpp file
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mousebutton_callback(GLFWwindow* window, int button, int action, int mods);
void mouseScrollWheel_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouseEnteredWindow_callback(GLFWwindow* window, int entered);
//
void AsyncKeyboardHandlingUpdate(GLFWwindow* window, cVAOManager &pVAOManager);
void AsyncMouseHandlingUpdate(GLFWwindow* window);


//struct sVertex
//{
//    float x, y, z;      // vPos  NOW WITH MORE Z!
//    float r, g, b;      // vCol
//};
// 
// See globalStuff.h for declaration
std::vector< cMesh* > g_vec_pMeshesToDraw;

cMesh* pMeshSphere = NULL;

cLightManager* pTheLightManager = NULL;
cBasicTextureManager* pTheTexureManager = NULL;


bool g_EnableDebugLightSpeheres = true;

unsigned int currentLight = 0;
double lastFrameTime;

glm::vec3 g_cameraEye = LoadCameraFromFile("assets/saves/CameraSaveFile.txt");
cFlyCamera* g_pFlyCamera = NULL;

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}




int main(void)
{
    GLFWwindow* window;
    GLuint shaderProgramNumber = 0;     

    glfwSetErrorCallback(error_callback);
	
    if ( ! glfwInit() )
    {
        //exit(EXIT_FAILURE);
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, key_callback);

    glfwSetMouseButtonCallback(window, mousebutton_callback);
    glfwSetScrollCallback(window, mouseScrollWheel_callback);
    glfwSetCursorEnterCallback(window, mouseEnteredWindow_callback);


    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);
    
    std::cout << "Starting GLFW context, OpenGL " << glGetString(GL_VERSION) << std::endl;

    // Use the shader manager thingy...
    cShaderManager* pShaderManager = new cShaderManager();
//    cShaderManager shaderManager;

    cShaderManager::cShader vertShader;
    vertShader.fileName = "simpleVertex.glsl";

    cShaderManager::cShader fragShader;
    fragShader.fileName = "simpleFragment.glsl";

    if ( ! pShaderManager->createProgramFromFile("basicShader", vertShader, fragShader) )
    {
        std::cout << "Oh no, there was an error!" << std::endl;
        std::cout << pShaderManager->getLastError() << std::endl;
        // exit
        return -1;
    }

    // Compiled and linked the shader program OK, so remember which program is was
    shaderProgramNumber = pShaderManager->getIDFromFriendlyName("basicShader");


    // Load some models
    cVAOManager* pVAOManager = new cVAOManager();
    std::string sceneLoadingErrors;
	
    //LoadModelDiagramAndModels("INFO3111P2_Olawale_Ayeni_1002834_103363.txt", pVAOManager, shaderProgramNumber, sceneLoadingErrors);


    if ( ! LoadAllTheModels("assets/saves/SaveFile.txt", pVAOManager, shaderProgramNumber, sceneLoadingErrors) )
    {
        std::cout << "Something went wrong: " << sceneLoadingErrors << std::endl;
    }
    else
    {
        std::cout << "Scene loaded OK" << std::endl;
    }
	

	
    std::string error;
    pTheLightManager = new cLightManager();
    pMeshSphere = new cMesh();

	
    GLint UseModelFileColour_UL = glGetUniformLocation(shaderProgramNumber, "bUseModelFileColours");
    glUniform1f(UseModelFileColour_UL, (GLfloat)GL_TRUE);


    // Fly camera. Uses mouse and keyboard to "fly" around the scene
    ::g_pFlyCamera = new cFlyCamera();
    g_pFlyCamera->eye = LoadCameraFromFile("assets/saves/CameraSaveFile.txt");

	
    double lastFrameTime = glfwGetTime();
	
	
    std::string lightErrors;
    if (!pTheLightManager->LoadLightConfigFromFile(*pTheLightManager, pVAOManager, shaderProgramNumber,"assets/saves/LightConfigSaveFile.txt", lightErrors)) {
        std::cout << "Light config load error: " << lightErrors << std::endl;
    }
    else {
        std::cout << "Light config loaded OK" << std::endl;
    }
	
    pTheTexureManager = new cBasicTextureManager();

    loadAllTextures(g_vec_pMeshesToDraw, pTheTexureManager);

    //pTheTexureManager->SetBasePath("assets/textures");
    //if (!pTheTexureManager->Create2DTextureFromBMPFile("WaterSurfaceTexture.bmp", true))
    //{
    //    std::cout << "Error: Didn't load WaterSurfaceTexture.bmp" << std::endl;
    //}
	

    //pTheTexureManager->SetBasePath("assets/textures/Dungeons");
    //if (!pTheTexureManager->Create2DTextureFromBMPFile("Dungeons_2_Texture_01_A_rotated_180_degrees.bmp", true))
    //{
    //    std::cout << "Error: Didn't load Dungeons_2_Texture_01_A_rotated_180_degrees.bmp" << std::endl;
    //}





    //pTheTexureManager->SetBasePath("assets/textures/Dungeons");
    //if (!pTheTexureManager->Create2DTextureFromBMPFile("Texture_01_A.bmp", true))
    //{
    //    std::cout << "Error: Didn't load Texture_01_A.bmp" << std::endl;
    //}
    //if (!pTheTexureManager->Create2DTextureFromBMPFile("Dungeons_2_Texture_01_A_rotated_180_degrees.bmp", true))
    //{
    //    std::cout << "Error: Didn't load Dungeons_2_Texture_01_A_rotated_180_degrees.bmp" << std::endl;
    //}
	
    //::pTheTexureManager->SetBasePath("assets/textures");
    //if (!::pTheTexureManager->Create2DTextureFromBMPFile("cat.bmp", true))
    //{
    //    std::cout << "Error: Didn't load cat.bmp" << std::endl;
    //}


    //█▀▄ █▀▀ ▄▀█ █▀█   █ █▀▄▀█ █▀▀ █░█ █   █▀▀ █▀█ █▄░█ ▀█▀ █▀▀ ▀▄▀ ▀█▀
    //█▄▀ ██▄ █▀█ █▀▄   █ █░▀░█ █▄█ █▄█ █   █▄▄ █▄█ █░▀█ ░█░ ██▄ █░█ ░█░
    // Setup Dear ImGui context



    lastFrameTime = glfwGetTime();
    pTheLightManager->bDayNight = true;
	
    while ( ! glfwWindowShouldClose(window) )
    {
        
        AsyncKeyboardHandlingUpdate(window, *pVAOManager);
        AsyncMouseHandlingUpdate(window);

		
        pTheLightManager->DayNightCycle(lastFrameTime);
        lastFrameTime = glfwGetTime();

        // How far away is the cow? 
        //std::cout << glm::distance(pCow->XYZLocation, ::g_pFlyCamera->getEye()) << std::endl;


        float ratio;
        int width, height;
//        mat4x4 m, p, mvp;     // linmath.h

        glm::mat4x4 matModel = glm::mat4(1.0f);
        glm::mat4x4 matProjection = glm::mat4(1.0f);
        glm::mat4x4 matView = glm::mat4(1.0f);

        glm::mat4x4 mvp = glm::mat4(1.0f);

        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float)height;
        if (ratio != ratio) {
            ratio = 1.0f;
        }

        glViewport(0, 0, width, height);
//        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.0f, 0.3f, 3.0f, 1.0f);       // Clear screen to blue colour
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//FOV
        matProjection = glm::perspective(0.6f,
                             ratio,
                             0.1f,
                             10000.0f);

        if (::bUseFlyCamera)
        {
            matView = glm::lookAt(::g_pFlyCamera->getEye(),
                                  ::g_pFlyCamera->getAt(),
                                  ::g_pFlyCamera->getUp());
        }
        else
        {
    //        glm::vec3 cameraEye = glm::vec3(0.0, 0.0, -4.0f);
            glm::vec3 cameraTarget = glm::vec3(0.0f, 5.0f, 0.0f);

            glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);

            //        mat4x4_mul(mvp, p, m);
        
            // HACK:
    //        unsigned int HACK_cow_index = 2;
    //        cameraTarget = ::g_vec_pMeshesToDraw[HACK_cow_index]->XYZLocation;

            matView = glm::lookAt( ::g_cameraEye,
                                   cameraTarget,
                                   upVector);
        }//if (::bUseFlyCamera)


        // Point the spotlight at the current target object
        // HACK:
        int target_index = pTheLightManager->theLights[currentLight].currentTarget;

        glm::vec3 rayFromTargetToLight
            = ::g_vec_pMeshesToDraw[target_index]->XYZLocation - glm::vec3(pTheLightManager->theLights[currentLight].position);
        rayFromTargetToLight = glm::normalize(rayFromTargetToLight);

        // INFO: Uncomment below to update the light direction
        // we can now use it to find the angle between the light direction
        //pTheLightManager->theLights[currentLight].direction = glm::vec4(rayFromTargetToLight, 1.0f);



//        glUseProgram(program);
        glUseProgram(shaderProgramNumber);


//        ::g_vec_pMeshesToDraw[4]->XYZLocation.z += 0.01f;

        // Place the light wherever the ball is...
//        pTheLightManager->theLights[0].position = glm::vec4(pMeshSphere->XYZLocation, 1.0f);
		
        // Copy the lighting information to the shader
        pTheLightManager->RandomizeLightAttens();
        CopyLightInformationToShader(pTheLightManager, shaderProgramNumber);



//    ____  _             _            __                                                _            
//   / ___|| |_ __ _ _ __| |_    ___  / _|  ___  ___ ___ _ __   ___   _ __ ___ _ __   __| | ___ _ __  
//   \___ \| __/ _` | '__| __|  / _ \| |_  / __|/ __/ _ \ '_ \ / _ \ | '__/ _ \ '_ \ / _` |/ _ \ '__| 
//    ___) | || (_| | |  | |_  | (_) |  _| \__ \ (_|  __/ | | |  __/ | | |  __/ | | | (_| |  __/ |    
//   |____/ \__\__,_|_|   \__|  \___/|_|   |___/\___\___|_| |_|\___| |_|  \___|_| |_|\__,_|\___|_|    
//                                                                                                    

        // Loop through the mesh objects in the scene, drawing each one
        // Checkpoint #8: Draw all the NON-transparent objects
        for (unsigned int index = 0; index != ::g_vec_pMeshesToDraw.size(); index++)
        {
            if (::g_vec_pMeshesToDraw[index]->RGBA.a >= 1.0f) {
                //sModelDrawInfo modelDrawingInfo;


                cMesh* pCurrentMesh = ::g_vec_pMeshesToDraw[index];
                ////HACK: FOR DISCARD TRASNP
                //if (index == 0) {
                //    // This is being used for discard transparency

					
                //    pCurrentMesh->friendlyName = "SeaFloor";
                //}
                //        mat4x4_identity(m);
                matModel = glm::mat4x4(1.0f);

                // All the object drawing code was here...
                DrawObject(pCurrentMesh, shaderProgramNumber, pVAOManager,
                    matModel, matView, matProjection);
            }
        }//for (unsigned int index = 0

         //Checkpoint #8: Sort them from "back to front" or "far to near" 
         // relative to the camera

         //Checkpoint #8: Draw all the TRANSPARENT objects.
		//make a new array of closest transparent objects using distance
		std::vector<cMesh*> vecTransparentObjects;
		for (unsigned int index = 0; index != ::g_vec_pMeshesToDraw.size(); index++)
		{
			if (::g_vec_pMeshesToDraw[index]->RGBA.a < 1.0f) {
                float dis = glm::distance(::g_vec_pMeshesToDraw[index]->XYZLocation, ::g_pFlyCamera->getEye());
				/*
                * if the object is closer than the current closest object,
                * then replace the current closest object with the new one
                */
				if(vecTransparentObjects.size() == 0)vecTransparentObjects.push_back(::g_vec_pMeshesToDraw[index]);
                else if(dis < glm::distance(vecTransparentObjects[0]->XYZLocation, ::g_pFlyCamera->getEye()))
				{
					vecTransparentObjects.push_back(::g_vec_pMeshesToDraw[index]);
				}
				else
				{
					vecTransparentObjects.insert(vecTransparentObjects.begin(), ::g_vec_pMeshesToDraw[index]);
				}
			}
		}

		//draw the transparent objects
		for (unsigned int index = 0; index != vecTransparentObjects.size(); index++)
		{
			cMesh* pCurrentMesh = vecTransparentObjects[index];

			//        mat4x4_identity(m);
			matModel = glm::mat4x4(1.0f);

			// All the object drawing code was here...
			DrawObject(pCurrentMesh, shaderProgramNumber, pVAOManager,
				matModel, matView, matProjection);
		}
	
//    _____           _          __                                                _            
//   | ____|_ __   __| |   ___  / _|  ___  ___ ___ _ __   ___   _ __ ___ _ __   __| | ___ _ __  
//   |  _| | '_ \ / _` |  / _ \| |_  / __|/ __/ _ \ '_ \ / _ \ | '__/ _ \ '_ \ / _` |/ _ \ '__| 
//   | |___| | | | (_| | | (_) |  _| \__ \ (_|  __/ | | |  __/ | | |  __/ | | | (_| |  __/ |    
//   |_____|_| |_|\__,_|  \___/|_|   |___/\___\___|_| |_|\___| |_|  \___|_| |_|\__,_|\___|_|    
//                                                                                              


// *********************************************************************
// Draw concentric spheres to represent the attenuation of the light

        if (::g_EnableDebugLightSpeheres )
        {
            for (int i = 0; i < pTheLightManager->numberOfUsedLights; ++i) {

                // Show the position of the light
                glm::mat4 matModelSphere = glm::mat4(1.0f);   // Identity matrix
                pMeshSphere->meshFileName = pTheLightManager->LIGHT_OBJECT_NAME;
                pMeshSphere->XYZLocation = pTheLightManager->theLights[i].position;
                pMeshSphere->bIsWireframe = true;
                pMeshSphere->bDisableBackFaceCulling = true;
                pMeshSphere->bIsNotLit = true;
                pMeshSphere->RGBA = glm::vec4(1.0f, 1.0f, 1.0, 1.0f);
                pMeshSphere->overallScale = 0.3f;
                DrawObject(pMeshSphere, shaderProgramNumber, pVAOManager,
                    matModelSphere, matView, matProjection);




                cLightHelper myLH;
                float distance90Percent = myLH.calcApproxDistFromAtten(0.9f,            // Desired light level
                    0.001f,           // Accuracy
                    1000000.0f,      // Infinite distance
                    pTheLightManager->theLights[i].atten.x,  // Const
                    pTheLightManager->theLights[i].atten.y,  // Linear
                    pTheLightManager->theLights[i].atten.z); // Quad
                pMeshSphere->RGBA = glm::vec4(0.6f, 0.0f, 0.0, 1.0f);
                pMeshSphere->overallScale = distance90Percent;
                DrawObject(pMeshSphere, shaderProgramNumber, pVAOManager,
                    matModelSphere, matView, matProjection);




                float distance50Percent = myLH.calcApproxDistFromAtten(0.5f,            // Desired light level
                    0.001f,           // Accuracy
                    1000000.0f,      // Infinite distance
                    pTheLightManager->theLights[i].atten.x,  // Const
                    pTheLightManager->theLights[i].atten.y,  // Linear
                    pTheLightManager->theLights[i].atten.z); // Quad
                pMeshSphere->RGBA = glm::vec4(0.6f, 0.6f, 0.0, 1.0f);
                pMeshSphere->overallScale = distance50Percent;
                DrawObject(pMeshSphere, shaderProgramNumber, pVAOManager,
                    matModelSphere, matView, matProjection);



                float distance25Percent = myLH.calcApproxDistFromAtten(0.25f,            // Desired light level
                    0.001f,           // Accuracy
                    1000000.0f,      // Infinite distance
                    pTheLightManager->theLights[i].atten.x,  // Const
                    pTheLightManager->theLights[i].atten.y,  // Linear
                    pTheLightManager->theLights[i].atten.z); // Quad
                pMeshSphere->RGBA = glm::vec4(0.0f, 0.6f, 0.0, 1.0f);
                pMeshSphere->overallScale = distance25Percent;
                DrawObject(pMeshSphere, shaderProgramNumber, pVAOManager,
                    matModelSphere, matView, matProjection);



                float distance1Percent = myLH.calcApproxDistFromAtten(0.01f,            // Desired light level
                    0.001f,           // Accuracy
                    1000000.0f,      // Infinite distance
                    pTheLightManager->theLights[i].atten.x,  // Const
                    pTheLightManager->theLights[i].atten.y,  // Linear
                    pTheLightManager->theLights[i].atten.z); // Quad
                pMeshSphere->RGBA = glm::vec4(0.0f, 0.6f, 0.6, 1.0f);
                pMeshSphere->overallScale = distance1Percent;
                DrawObject(pMeshSphere, shaderProgramNumber, pVAOManager,
                    matModelSphere, matView, matProjection);
            }

        } //if (::g_EnableDebugLightSpeheres)


// *********************************************************************

        //pMeshSphere->XYZLocation.x = -10.0f;
        //pMeshSphere->XYZLocation.y = 0.0f;
        //pMeshSphere->bIsWireframe = false;
        //pMeshSphere->bDisableBackFaceCulling = false;
        //pMeshSphere->bIsNotLit = false;
        //DrawObject(pMeshSphere, shaderProgramNumber, pVAOManager,
        //           matModelSphere, matView, matProjection);



//    ____                           _     _                                       
//   |  _ \ _ __ ___  ___  ___ _ __ | |_  | |_ ___    ___  ___ _ __ ___  ___ _ __  
//   | |_) | '__/ _ \/ __|/ _ \ '_ \| __| | __/ _ \  / __|/ __| '__/ _ \/ _ \ '_ \ 
//   |  __/| | |  __/\__ \  __/ | | | |_  | || (_) | \__ \ (__| | |  __/  __/ | | |
//   |_|   |_|  \___||___/\___|_| |_|\__|  \__\___/  |___/\___|_|  \___|\___|_| |_|
//                                                                                 
        glfwSwapBuffers(window);
        glfwPollEvents();

        // Change the title of the window to show the camera location
        std::stringstream ssTitle;
        ssTitle << "Camera (x,y,z): "               // just like cout or fstream, etc.
            << ::g_pFlyCamera->eye.x << ", "
            << ::g_pFlyCamera->eye.y << ", "
            << ::g_pFlyCamera->eye.z;
        ssTitle << "Camera Lookat (x,y,z): "
            << ::g_pFlyCamera->getAt().x << ", "
            << ::g_pFlyCamera->getAt().y << ", "
            << ::g_pFlyCamera->getAt().z;
        glfwSetWindowTitle(window, ssTitle.str().c_str());
//        glfwSetWindowTitle(window, "Hello!");
        glfwSetWindowTitle( window, ssTitle.str().c_str() );

    }// while ( ! glfwWindowShouldClose(window) )

    // Save my scene...


    glfwDestroyWindow(window);

    // Clean up
    delete pShaderManager;
    delete pVAOManager;
    delete pTheLightManager;
    delete ::g_pFlyCamera;

    glfwTerminate();
    //exit(EXIT_SUCCESS);
    return 0;
}


void PlaceAModelInARandomLocation(std::string modelName, float minScale, float maxScale)
{
    cMesh* pDeLorean = new cMesh();
    pDeLorean->meshFileName = modelName;// "assets/models/mig29_xyz_n_rgba_uv.ply";
//    pDeLorean->bIsWireframe = true;
    pDeLorean->orientationEulerAngle.x = getRandBetween(-3.14159f, 3.14159f);
    pDeLorean->orientationEulerAngle.y = getRandBetween(-3.14159f, 3.14159f);
    pDeLorean->orientationEulerAngle.z = getRandBetween(-3.14159f, 3.14159f);
    // Maybe random colours, too
    pDeLorean->RGBA.r = getRandBetween(0.0f, 1.0f);
    pDeLorean->RGBA.g = getRandBetween(0.0f, 1.0f);
    pDeLorean->RGBA.b = getRandBetween(0.0f, 1.0f);
    pDeLorean->overallScale = getRandBetween(minScale, maxScale);
    pDeLorean->XYZLocation.x = getRandBetween(-100.0f, 100.0f);
    pDeLorean->XYZLocation.y = getRandBetween(-100.0f, 100.0f);
    pDeLorean->XYZLocation.z = getRandBetween(-100.0f, 100.0f);
    ::g_vec_pMeshesToDraw.push_back(pDeLorean);
    return;
}

void CopyLightInformationToShader(cLightManager* pTheLightManager, GLuint shaderID)
{
    for (int i = 0; i < pTheLightManager->NUMBEROFLIGHTS; i++) {
        std::stringstream s;
        s << "theLights[" << i << "].position";
        GLint position_UniLoc = glGetUniformLocation(shaderID, s.str().c_str());
        glUniform4f(position_UniLoc,
            pTheLightManager->theLights[i].position.x,
            pTheLightManager->theLights[i].position.y,
            pTheLightManager->theLights[i].position.z,
            pTheLightManager->theLights[i].position.w);

        s.clear();
        s.str(std::string());
        s << "theLights[" << i << "].diffuse";
        GLint diffuse_UniLoc = glGetUniformLocation(shaderID, s.str().c_str());
        glUniform4f(diffuse_UniLoc,
            pTheLightManager->theLights[i].diffuse.r,
            pTheLightManager->theLights[i].diffuse.g,
            pTheLightManager->theLights[i].diffuse.b,
            pTheLightManager->theLights[i].diffuse.a);

        s.clear();
        s.str(std::string());
        s << "theLights[" << i << "].specular";
        GLint specular_UniLoc = glGetUniformLocation(shaderID, s.str().c_str());
        glUniform4f(specular_UniLoc,
            pTheLightManager->theLights[i].specular.r,
            pTheLightManager->theLights[i].specular.g,
            pTheLightManager->theLights[i].specular.b,
            pTheLightManager->theLights[i].specular.a);

        s.clear();
        s.str(std::string());
        s << "theLights[" << i << "].atten";
        GLint atten_UniLoc = glGetUniformLocation(shaderID, s.str().c_str());
        glUniform4f(atten_UniLoc,
            pTheLightManager->theLights[i].atten.r,
            pTheLightManager->theLights[i].atten.g,
            pTheLightManager->theLights[i].atten.b,
            pTheLightManager->theLights[i].atten.a);

        s.clear();
        s.str(std::string());
        s << "theLights[" << i << "].direction";
        GLint direction_UniLoc = glGetUniformLocation(shaderID, s.str().c_str());
        glUniform4f(direction_UniLoc,
            pTheLightManager->theLights[i].direction.r,
            pTheLightManager->theLights[i].direction.g,
            pTheLightManager->theLights[i].direction.b,
            pTheLightManager->theLights[i].direction.a);

        s.clear();
        s.str(std::string());
        s << "theLights[" << i << "].param1";
        GLint param1_UniLoc = glGetUniformLocation(shaderID, s.str().c_str());
        glUniform4f(param1_UniLoc,
            pTheLightManager->theLights[i].param1.x,
            pTheLightManager->theLights[i].param1.y,
            pTheLightManager->theLights[i].param1.z,
            pTheLightManager->theLights[i].param1.w);

        s.clear();
        s.str(std::string());
        s << "theLights[" << i << "].param2";
        GLint param2_UniLoc = glGetUniformLocation(shaderID, s.str().c_str());
        glUniform4f(param2_UniLoc,
            pTheLightManager->theLights[i].param2.x,
            pTheLightManager->theLights[i].param2.y,
            pTheLightManager->theLights[i].param2.z,
            pTheLightManager->theLights[i].param2.w);
    }

    return;
}