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
#include "UI.h"

#include "dear/imgui.h"
#include "dear/imgui_impl_glfw.h"
#include "dear/imgui_impl_opengl3.h"


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

bool g_EnableDebugLightSpeheres = true;

unsigned int currentLight = 0;

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






	

    //ImGui::InputText("string", buf, IM_ARRAYSIZE(buf));
    //ImGui::SliderFloat("float", &f, 0.0f, 1.0f);

//    ::g_vec_pMeshesToDraw.push_back(pMeshSphere);


//    cMesh* pSpider1 = new cMesh();
//    pSpider1->XYZLocation.x = -5.07f;
//    pSpider1->orientationEulerAngle.x = glm::radians(-137.5f);
//    pSpider1->orientationEulerAngle.z = glm::radians(+8.14f);
//    pSpider1->RGBA = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
//    pSpider1->meshFileName = "assets/models/mig29_xyz_n_rgba_uv.ply";
//    ::g_vec_pMeshesToDraw.push_back(pSpider1);
//
//    cMesh* pSpider2 = new cMesh();
//    pSpider2->XYZLocation.x = +5.0f;
//    pSpider2->XYZLocation.y = 2.0f;
//    pSpider2->RGBA = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);
//    pSpider2->meshFileName = "assets/models/mig29_xyz_n_rgba_uv.ply";
//    pSpider2->overallScale = 3.0f;
////    pSpider2->bIsWireframe = true;      // glPolygonMode() will be GL_LINE
//    ::g_vec_pMeshesToDraw.push_back(pSpider2);
//
//    cMesh* pCow = new cMesh();
//    pCow->orientationEulerAngle.y = glm::radians(-145.0f);
//    pCow->overallScale = 0.5f;
//    pCow->RGBA = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
////    pCow->RGBA = glm::vec4(0.5f, 1.0f, 0.5f, 1.0f);
////    pCow->meshFileName = "assets/models/cow_xyz_only.ply";
//    pCow->meshFileName = "assets/models/cow_xyz_n_rgba_uv.ply";
//    ::g_vec_pMeshesToDraw.push_back(pCow);
//
//    cMesh* pSeaFloor = new cMesh();
//    pSeaFloor->XYZLocation.y = -7.0f;
//    // https://www.colorhexa.com/c2b280
//    pSeaFloor->RGBA = glm::vec4(76.0f/255.0f, 70.0f/255.0f, 50.0f/255.0f, 1.0f);
//    pSeaFloor->meshFileName = "assets/models/Seafloor2_xyz_n_rgba_uv.ply";
//    ::g_vec_pMeshesToDraw.push_back(pSeaFloor);
//
//    cMesh* pssj100 = new cMesh();
//    pssj100->XYZLocation.z = 15.0f;
//    // rgb(244, 136, 78)
//    pssj100->RGBA = glm::vec4(244.0f/255.0f, 136.0f/255.0f, 78.0f/255.0f, 1.0f);
//    pssj100->orientationEulerAngle.y = glm::radians(180.0f);
//    //pssj100->overallScale = 25.0f;
//    pssj100->meshFileName = "assets/models/ssj100_xyz_n_rgba_uv.ply";
//    ::g_vec_pMeshesToDraw.push_back(pssj100);
//
//
//    cMesh* pWarehouse = new cMesh();
////    pWarehouse->XYZLocation.z = 15.0f;
//    pWarehouse->bUseModelFileColours = true;
//    pWarehouse->orientationEulerAngle.y = glm::radians(90.0f);
//    pWarehouse->meshFileName = "assets/models/Warehouse_xyz_n_rgba_uv.ply";
//    ::g_vec_pMeshesToDraw.push_back(pWarehouse);
    


    // Make a bunch of airplanes
//    for (unsigned int count = 0; count != 1000; count++)
//    {
//        PlaceAModelInARandomLocation("assets/models/mig29_xyz_n_rgba_uv.ply", 1.0f, 5.0f);
//    }

//    float extent = 50.0f;
//    float step = 20.0f;
//
//    for ( float x = -extent; x <= extent; x += step )
//    {
//        for (float y = -extent; y <= extent; y += step)
//        {
//            for (float z = -extent; z <= extent; z += step)
//            {
//                cMesh* pMig = new cMesh();
//                pMig->XYZLocation = glm::vec3(x, y, z);
//                pMig->RGBA = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
//                pMig->meshFileName = "assets/models/mig29_xyz_n_rgba_uv.ply";
//                pMig->overallScale = 3.0f;
//                ::g_vec_pMeshesToDraw.push_back(pMig);
//            }//for (float z
//        }//for (float y
//    }//for ( float x



//    pMeshSphere->RGBA = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
//    pMeshSphere->XYZLocation = glm::vec3(0.0f, 15.0f, 0.0f);
//    pMeshSphere->meshFileName = "assets/models/Isoshphere_flat_4div_xyz_n_rgba_uv.ply";
//    pMeshSphere->bIsWireframe = true;
//    // Note: I'm NOT including this in the vector of objects to draw
//

//
//    pTheLightManager->theLights[0].position = glm::vec4(0.0f, 15.0f, 0.0f, 1.0f);
////    pTheLightManager->theLights[0].position = glm::vec4(0.0f, 15.0f, 0.0f, 1.0f);
//    pTheLightManager->theLights[0].diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
//
//    pTheLightManager->theLights[0].atten.x = 0.0f;  // Constant
//    pTheLightManager->theLights[0].atten.y = 0.05f;  // Linear attenuation
//    pTheLightManager->theLights[0].atten.z = 0.01f;  // Quadratic attenuation
//
//    pTheLightManager->theLights[0].param2.x = 1.0f;  // Turn the light on
//
//    pTheLightManager->theLights[0].diffuse.r = 1.0f;
//    pTheLightManager->theLights[0].diffuse.g = 1.0f;
//    pTheLightManager->theLights[0].diffuse.b = 1.0f;
//
//    pTheLightManager->theLights[0].param1.x = 0;    // 0 = Point light, 1 = spot
//    // If it's a spot light, then we need some angles
//    pTheLightManager->theLights[0].param1.y = 15.0f; // y = inner angle 
//    pTheLightManager->theLights[0].param1.z = 15.0f; // z = outer angle
//    // Point the spot straight down (y is the height axis)
//    pTheLightManager->theLights[0].direction.x =  0.0f;
//    pTheLightManager->theLights[0].direction.y = -1.0f;
//    pTheLightManager->theLights[0].direction.z =  0.0f;



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
	



    //█▀▄ █▀▀ ▄▀█ █▀█   █ █▀▄▀█ █▀▀ █░█ █   █▀▀ █▀█ █▄░█ ▀█▀ █▀▀ ▀▄▀ ▀█▀
    //█▄▀ ██▄ █▀█ █▀▄   █ █░▀░█ █▄█ █▄█ █   █▄▄ █▄█ █░▀█ ░█░ ██▄ █░█ ░█░
    // Setup Dear ImGui context
    const char* glsl_version = "#version 410";
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    io.IniFilename = NULL;
    // Setup Dear ImGui style

    UI::InitChanges();
	
    ImGui::StyleColorsDark();

    while ( ! glfwWindowShouldClose(window) )
    {

        AsyncKeyboardHandlingUpdate(window, *pVAOManager);
        AsyncMouseHandlingUpdate(window);

        double thisFrameTime = glfwGetTime();
        double deltaTime = lastFrameTime - thisFrameTime;
        const double MAX_DELTA_TIME = 0.1f;     // 100 ms
        if ( deltaTime > MAX_DELTA_TIME )
        {
            deltaTime = MAX_DELTA_TIME;
        }

        ::g_pFlyCamera->Update(deltaTime);
        UpdateEachFrame(deltaTime);

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


        //█▀▄ █▀▀ ▄▀█ █▀█   █ █▀▄▀█ █▀▀ █░█ █   █▄▄ █▀█ █ █░░ █▀▀ █▀█   █▀█ █░░ ▄▀█ ▀█▀ █▀▀
        //█▄▀ ██▄ █▀█ █▀▄   █ █░▀░█ █▄█ █▄█ █   █▄█ █▄█ █ █▄▄ ██▄ █▀▄   █▀▀ █▄▄ █▀█ ░█░ ██▄
        // feed inputs to dear imgui, start new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        //█▀▄ █▀▀ ▄▀█ █▀█   █ █▀▄▀█ █▀▀ █░█ █   █▀▀ █▀█ █▀▀ ▄▀█ ▀█▀ █ █▀█ █▄░█   ▄▀█ █▀█ █▀▀ ▄▀█
        //█▄▀ ██▄ █▀█ █▀▄   █ █░▀░█ █▄█ █▄█ █   █▄▄ █▀▄ ██▄ █▀█ ░█░ █ █▄█ █░▀█   █▀█ █▀▄ ██▄ █▀█
        // render your GUI
        //ImGui::Text("Hello, world %d", 123);
        //if (ImGui::Button("Save"))
        //    SaveSceneToFile("assets/saves/SaveFile.txt", error);
        //ImGui::ShowDemoWindow();
		UI::ShowMainWindow();




        //█▀▄ █▀▀ ▄▀█ █▀█   █ █▀▄▀█ █▀▀ █░█ █   █▀▀ █▄░█ █▀▄
        //█▄▀ ██▄ █▀█ █▀▄   █ █░▀░█ █▄█ █▄█ █   ██▄ █░▀█ █▄▀
		
//        mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
        matProjection = glm::perspective(0.6f,
                             ratio,
                             0.1f,
                             1000.0f);

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

    //        // uniform vec3 eyeLocation;
    //        GLint eyeLocation_UniformID = glGetUniformLocation(shaderProgramNumber, "eyeLocation");
    //        glUniform3f(eyeLocation_UniformID,
    //                    ::g_cameraEye.x, ::g_cameraEye.y, ::g_cameraEye.z);
        }//if (::bUseFlyCamera)


        // Point the spotlight at the cow
        // Ray from the cow to the light
        // HACK:
        int HACK_cow_index = pTheLightManager->theLights[currentLight].currentTarget;

        glm::vec3 rayFromCowToLight
            = ::g_vec_pMeshesToDraw[HACK_cow_index]->XYZLocation - glm::vec3(pTheLightManager->theLights[currentLight].position);
        rayFromCowToLight = glm::normalize(rayFromCowToLight);

        // rayFromCowToLight is now length 1.0 (or it's "normalized");
        pTheLightManager->theLights[currentLight].direction = glm::vec4(rayFromCowToLight, 1.0f);



//        glUseProgram(program);
        glUseProgram(shaderProgramNumber);


//        ::g_vec_pMeshesToDraw[4]->XYZLocation.z += 0.01f;

        // Place the light wherever the ball is...
//        pTheLightManager->theLights[0].position = glm::vec4(pMeshSphere->XYZLocation, 1.0f);
		
        // Copy the lighting information to the shader
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
            if (::g_vec_pMeshesToDraw[index]->RGBA.a == 1.0f) {
                //sModelDrawInfo modelDrawingInfo;

                cMesh* pCurrentMesh = ::g_vec_pMeshesToDraw[index];

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
		
    //█▀▄ █▀▀ ▄▀█ █▀█   █ █▀▄▀█ █▀▀ █░█ █   █▀█ █▀▀ █▄░█ █▀▄ █▀▀ █▀█
    //█▄▀ ██▄ █▀█ █▀▄   █ █░▀░█ █▄█ █▄█ █   █▀▄ ██▄ █░▀█ █▄▀ ██▄ █▀▄
    // Render dear imgui into screen
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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
            << ::g_cameraEye.x << ", "
            << ::g_cameraEye.y << ", "
            << ::g_cameraEye.z;

        ssTitle << " Light#" << currentLight << "Current Target: "
			<< pTheLightManager->theLights[currentLight].currentTarget << ", "
            << "XYZ: "
            << pTheLightManager->theLights[currentLight].position.x << ", "
            << pTheLightManager->theLights[currentLight].position.y << ", "
            << pTheLightManager->theLights[currentLight].position.z << "  "
            << "Linear: " << pTheLightManager->theLights[currentLight].atten.y
            << " Quad: " << pTheLightManager->theLights[currentLight].atten.z
            << " (in,out) angle: ("
            << pTheLightManager->theLights[currentLight].param1.y << ", "
            << pTheLightManager->theLights[currentLight].param1.z << ")";
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