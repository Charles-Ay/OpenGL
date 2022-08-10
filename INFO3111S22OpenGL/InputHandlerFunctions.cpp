// InputHandlerFunctions.cpp

#include "globalStuff.h"    // For GFLW header

#include <iostream>

// g_cameraEye is defined in theMain.cpp
extern glm::vec3 g_cameraEye;

bool bUseFlyCamera = true;

// This is so the fly camera won't "pay attention" to the mouse if it's 
//  off the output window
bool g_IsMouseInsideWindow = false;

// This function is defined in theMain.cpp
void PlaceAModelInARandomLocation(std::string modelName, float minScale, float maxScale);

void mouseScrollWheel_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    float mouseScrollWheelSensitivity = 0.1f;
    ::g_pFlyCamera->setMouseWheelDelta(yoffset * mouseScrollWheelSensitivity);
    return;
}

void mouseEnteredWindow_callback(GLFWwindow* window, int entered)
{
    if ( entered )
    {
        ::g_IsMouseInsideWindow = true;
    }
    else
    {
        ::g_IsMouseInsideWindow = false;
    }
    return;
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    // WASD - movement 
    // A = left
    // D = right
    // W = forward
    // S = backward
    // Q = down
    // E = up


//    if ( mods == GLFW_MOD_SHIFT )       // ONLY the shift and NO OTHER MODIFIERS
//    if ( ( mods & GLFW_MOD_SHIFT ) == GLFW_MOD_SHIFT )      // The shift and maybe other modifers, too
//        0000111
//        0000001 
//        -------
//        0000001

    const float cameraMovementSpeed = 1.0f;
    const float objectMovementSpeed = 0.5f;
    const float objectRotationSpeed = glm::radians(1.0f);

    // Nudge, nudge, wink, wink - Hey, why not change this value with some other keys...
    //  then maybe you can "select" all the objects?? Huh??? Isn't THAT neat??
    unsigned int objectIDToMove = 2;



    if (mods != GLFW_MOD_SHIFT && mods != GLFW_MOD_CONTROL && mods != GLFW_MOD_ALT)
    {
        // NO modifier keys are pressed

        // When we press #8, a airplane is added to the scene
        //if (key == GLFW_KEY_8)
        //{
        //    PlaceAModelInARandomLocation("assets/models/mig29_xyz_n_rgba_uv.ply", 1.0f, 5.0f);
        //}

        // Saves the scene to a file
        if (key == GLFW_KEY_9 && action == GLFW_PRESS)
        {
            std::string error;
            SaveSceneToFile("assets/saves/SaveFile.txt", error);
            //std::cout << "Saved info. to file" << std::endl;
        }

        if (key == GLFW_KEY_PAGE_UP && action == GLFW_PRESS) 
        {
            ::g_EnableDebugLightSpeheres = true;
        }
        if (key == GLFW_KEY_PAGE_DOWN && action == GLFW_PRESS)
        {
            ::g_EnableDebugLightSpeheres = false;
        }

    }

    return;
}

void mousebutton_callback(GLFWwindow* window, int button, int action, int mods)
{
    switch (button)
    {
    case GLFW_MOUSE_BUTTON_1:
        std::cout << "GLFW_MOUSE_BUTTON_1" << std::endl;
        break;
    case GLFW_MOUSE_BUTTON_2:
        std::cout << "GLFW_MOUSE_BUTTON_2" << std::endl;
        break;
    case GLFW_MOUSE_BUTTON_3:
        std::cout << "GLFW_MOUSE_BUTTON_3" << std::endl;
        break;
    }
    return;
}

bool AreAllModifiersUp(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) { return false; }
    if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) { return false; }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) { return false; }
    if (glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS) { return false; }
    if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) { return false; }
    if (glfwGetKey(window, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS) { return false; }

    return true;
}



// This gets called every frame to handle async keyboard inputs
void AsyncKeyboardHandlingUpdate(GLFWwindow* window, cVAOManager& pVAOManager)
{
    try {


        //int state = glfwGetKey(window, GLFW_KEY_E);
        //if ( state == GLFW_PRESS )
        //{
        //    std::cout << "E is pressed" << std::endl;
        //}

        const float cameraMovementSpeed = 1.0f;
        const float cameraTurnSpeed = 0.1f; // degrees
        const float objectMovementSpeed = 0.5f;
        const float objectRotationSpeed = glm::radians(1.0f);

        // Nudge, nudge, wink, wink - Hey, why not change this value with some other keys...
        //  then maybe you can "select" all the objects?? Huh??? Isn't THAT neat??
        unsigned int objectIDToMove = 2;

        if ( glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS )
        {
            // Move the object
    //        if (key == GLFW_KEY_A) { ::g_vec_pMeshesToDraw[objectIDToMove]->XYZLocation.x -= objectMovementSpeed; }
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) 
            { ::g_vec_pMeshesToDraw[objectIDToMove]->XYZLocation.x -= objectMovementSpeed; }
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) 
            { ::g_vec_pMeshesToDraw[objectIDToMove]->XYZLocation.x += objectMovementSpeed; }
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            {
                ::g_vec_pMeshesToDraw[objectIDToMove]->XYZLocation.z -= objectMovementSpeed;
            }
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            {
                ::g_vec_pMeshesToDraw[objectIDToMove]->XYZLocation.z += objectMovementSpeed;
            }
            if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
            {
                ::g_vec_pMeshesToDraw[objectIDToMove]->XYZLocation.y -= objectMovementSpeed;
            }
            if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
            {
                ::g_vec_pMeshesToDraw[objectIDToMove]->XYZLocation.y += objectMovementSpeed;
            }

            //if (key == GLFW_KEY_D) { ::g_vec_pMeshesToDraw[objectIDToMove]->XYZLocation.x += objectMovementSpeed; }
            //if (key == GLFW_KEY_W) { ::g_vec_pMeshesToDraw[objectIDToMove]->XYZLocation.z -= objectMovementSpeed; }
            //if (key == GLFW_KEY_S) { ::g_vec_pMeshesToDraw[objectIDToMove]->XYZLocation.z += objectMovementSpeed; }
            //if (key == GLFW_KEY_Q) { ::g_vec_pMeshesToDraw[objectIDToMove]->XYZLocation.y -= objectMovementSpeed; }
            //if (key == GLFW_KEY_E) { ::g_vec_pMeshesToDraw[objectIDToMove]->XYZLocation.y += objectMovementSpeed; }
        }
        if ( glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS )
        {
            // Move the object
            if (glfwGetKey(window, GLFW_KEY_A)) { ::g_vec_pMeshesToDraw[objectIDToMove]->orientationEulerAngle.x -= objectRotationSpeed; }
            if (glfwGetKey(window, GLFW_KEY_D)) { ::g_vec_pMeshesToDraw[objectIDToMove]->orientationEulerAngle.x += objectRotationSpeed; }
            if (glfwGetKey(window, GLFW_KEY_W)) { ::g_vec_pMeshesToDraw[objectIDToMove]->orientationEulerAngle.z -= objectRotationSpeed; }
            if (glfwGetKey(window, GLFW_KEY_S)) { ::g_vec_pMeshesToDraw[objectIDToMove]->orientationEulerAngle.z += objectRotationSpeed; }
            if (glfwGetKey(window, GLFW_KEY_Q)) { ::g_vec_pMeshesToDraw[objectIDToMove]->orientationEulerAngle.y -= objectRotationSpeed; }
            if (glfwGetKey(window, GLFW_KEY_E)) { ::g_vec_pMeshesToDraw[objectIDToMove]->orientationEulerAngle.y += objectRotationSpeed; }
        }
        if ( glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS )
        {
            // Move the object
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) { pTheLightManager->theLights[currentLight].position.x -= objectMovementSpeed; }
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) { pTheLightManager->theLights[currentLight].position.x += objectMovementSpeed; }
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) { pTheLightManager->theLights[currentLight].position.z -= objectMovementSpeed; }
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) { pTheLightManager->theLights[currentLight].position.z += objectMovementSpeed; }
            if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) { pTheLightManager->theLights[currentLight].position.y -= objectMovementSpeed; }
            if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) { pTheLightManager->theLights[currentLight].position.y += objectMovementSpeed; }
            //if (key == GLFW_KEY_A) { pMeshSphere->XYZLocation.x -= objectMovementSpeed; }
            //if (key == GLFW_KEY_D) { pMeshSphere->XYZLocation.x += objectMovementSpeed; }
            //if (key == GLFW_KEY_W) { pMeshSphere->XYZLocation.z -= objectMovementSpeed; }
            //if (key == GLFW_KEY_S) { pMeshSphere->XYZLocation.z += objectMovementSpeed; }
            //if (key == GLFW_KEY_Q) { pMeshSphere->XYZLocation.y -= objectMovementSpeed; }
            //if (key == GLFW_KEY_E) { pMeshSphere->XYZLocation.y += objectMovementSpeed; }


            // Linear attenuation
            if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
                pTheLightManager->theLights[currentLight].atten.y *= 0.99f;
            }
            if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
                pTheLightManager->theLights[currentLight].atten.y *= 1.01f;
            }
            // Quadradic attenuation
            if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
                pTheLightManager->theLights[currentLight].atten.z *= 0.99f;
            }
            if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
                pTheLightManager->theLights[currentLight].atten.z *= 1.01f;
            }

    //        vec4 param1;	// x = lightType, y = inner angle, z = outer angle, w = TBD
    //                        // 0 = pointlight
    //                        // 1 = spot light
    //                        // 2 = directional light
            if (glfwGetKey(window, GLFW_KEY_LEFT_BRACKET) == GLFW_PRESS)
            {   // Make it a point light
                pTheLightManager->theLights[currentLight].param1.x = 0;
            }
            if (glfwGetKey(window, GLFW_KEY_RIGHT_BRACKET) == GLFW_PRESS)
            {   // Make it a spot light
                pTheLightManager->theLights[currentLight].param1.x = 1;
            }
        
            // Change the spot light angle 
            //const float spotlightAngleChange = 0.01f;
            //if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)
            //{   // Make the OUTER angle LESS
            //    pTheLightManager->theLights[currentLight].param1.z -= spotlightAngleChange;
            //}
            //if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
            //{   // Make the OUTER angle MORE
            //    pTheLightManager->theLights[currentLight].param1.z += spotlightAngleChange;
            //}

            //if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS)
            //{   // Make the INNER angle LESS
            //    pTheLightManager->theLights[currentLight].param1.y -= spotlightAngleChange;
            //}
            //if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS)
            //{   // Make the INNER angle MORE
            //    pTheLightManager->theLights[currentLight].param1.y += spotlightAngleChange;
            //}
            //if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
            //{   // Go to next light
            //    if (currentLight < pTheLightManager->numberOfUsedLights - 1)
            //    {
            //        currentLight++;
            //    }
            //    else currentLight = 0;
            //}            
            if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
            {   // Change Target
                pTheLightManager->theLights[currentLight].ChangeTargetToLookAt();
            }
            // Some checks about these angles
        
            // TODO: Maybe check that the angles are > 0.0f and < some angle (like 90 degrees??)

            if (pTheLightManager->theLights[currentLight].param1.z < pTheLightManager->theLights[currentLight].param1.y )
            {
                pTheLightManager->theLights[currentLight].param1.z = pTheLightManager->theLights[currentLight].param1.y;
            }
            if (pTheLightManager->theLights[currentLight].param1.y > pTheLightManager->theLights[currentLight].param1.z )
            {
                pTheLightManager->theLights[currentLight].param1.y = pTheLightManager->theLights[currentLight].param1.z;
            }


        }//if ( glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS )
   
        if ( AreAllModifiersUp( window ) )
        {
            // What kind of camera are we using?
            if (bUseFlyCamera)
            {
                // Left and Right
                if ( glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS ) { ::g_pFlyCamera->MoveLeftRight_X(-cameraMovementSpeed); }
                if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)   { ::g_pFlyCamera->MoveLeftRight_X(+cameraMovementSpeed); }

                // Forward and backwards
                if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) { 
                    ::g_pFlyCamera->MoveForward_Z(+cameraMovementSpeed); }
                if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) { 
                    ::g_pFlyCamera->MoveForward_Z(-cameraMovementSpeed); }

                // Up and down
                if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) { ::g_pFlyCamera->MoveUpDown_Y(-cameraMovementSpeed); }
                if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) { ::g_pFlyCamera->MoveUpDown_Y(+cameraMovementSpeed); }
            
            }
            else
            {

                // Camera control
                if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
                {
                    ::g_cameraEye.x -= cameraMovementSpeed;     // "left"
            //        ::g_vec_pMeshesToDraw[4]->XYZLocation.x -= cameraMovementSpeed;     // "left"
                }
                if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
                {
                    ::g_cameraEye.x += cameraMovementSpeed;     // "right"
            //        ::g_vec_pMeshesToDraw[4]->XYZLocation.x += cameraMovementSpeed;     // "left"
                }

                if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
                {
                    ::g_cameraEye.y -= cameraMovementSpeed;     // "down"
                }
                if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
                {
                    ::g_cameraEye.y += cameraMovementSpeed;     // "up"
                }

                if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
                {
                    ::g_cameraEye.z += cameraMovementSpeed;     // "forward"
                }
                if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
                {
                    ::g_cameraEye.z -= cameraMovementSpeed;     // "backwards"
                }// We AREN'T holding JUST the shift key

            }// if (bUseFlyCamera)
            //if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
            //    std::string error;
            //    pTheLightManager->SaveLightConfigToFile(*pTheLightManager, "assets/saves/LightConfigSaveFile.txt", error);
            //    std::cout << "Lights saved to file";
            //}
            //if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
            //    std::string error;
            //    SaveSceneToFile("assets/saves/SaveFile.txt", error);
            //    std::cout << "Scene saved to file";
            //}
            //if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
            //    std::string error;
            //    SaveCamaeraToFile("assets/saves/CameraSaveFile.txt");
            //    std::cout << "Camera saved to file";
            //}
        }// if ( AreAllModifiersUp( window ) )

    }
    catch (std::exception e) {
        std::cout << e.what();
    }
    return;
}

void AsyncMouseHandlingUpdate(GLFWwindow* window)
{

    double x, y;
    glfwGetCursorPos(window, &x, &y);

    ::g_pFlyCamera->setMouseXY(x, y);

    const float MOUSE_MOVE_SENSITIVITY = 0.1f;

    if ( ( glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) && ::g_IsMouseInsideWindow )
    {

        // If mouse button is down, then aim the camera
        ::g_pFlyCamera->Yaw_LeftRight(-::g_pFlyCamera->getDeltaMouseX() * MOUSE_MOVE_SENSITIVITY);

        ::g_pFlyCamera->Pitch_UpDown(::g_pFlyCamera->getDeltaMouseY() * MOUSE_MOVE_SENSITIVITY);

    }//if ( ( glfwGetMouseButton...

    // Adjust the mouse move speed 
    // TODO: This is pretty janky....
    if ( ::g_IsMouseInsideWindow )
    {

        const float MOUSE_WHEEL_SENSITIVITY = 0.1f;

        ::g_pFlyCamera->movementSpeed += (::g_pFlyCamera->getMouseWheel() * MOUSE_WHEEL_SENSITIVITY);

        // We are looking for the CHANGE in the mouse wheel, but it's storing the 
        //  absolute value of the current mouse wheel
        ::g_pFlyCamera->clearMouseWheelValue();

        // Don't let the speed change go negative
        if ( ::g_pFlyCamera->movementSpeed <= 0.0f )
        {
            ::g_pFlyCamera->movementSpeed = 0.0f;
        }
    }//if ( ::g_IsMouseInsideWindow )

    return;
}
