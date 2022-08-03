// DrawObjectFunction.cpp

#include "globalStuff.h"        // glad and GFLW (i.e. OpenGL call stuff)
#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> 
// glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

void DrawObject(cMesh* pCurrentMesh, GLuint shaderProgramNumber, cVAOManager* pVAOManager,
    glm::mat4 matModel, glm::mat4 matView, glm::mat4 matProjection)
{

    // Is visible?
    if (!pCurrentMesh->bIsVisible)
    {
        // Don't draw. 
        return;
    }

    //        mat4x4_rotate_Z(m, m, (float)glfwGetTime());
    glm::mat4 rotateX = glm::rotate(glm::mat4(1.0f),
        pCurrentMesh->orientationEulerAngle.x,
        glm::vec3(1.0f, 0.0f, 0.0f));

    glm::mat4 rotateY = glm::rotate(glm::mat4(1.0f),
        pCurrentMesh->orientationEulerAngle.y,           //glm::radians(147.0f),  
        glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 rotateZ = glm::rotate(glm::mat4(1.0f),
        pCurrentMesh->orientationEulerAngle.z,  // (float)glfwGetTime(),           //glm::radians(147.0f),  
        glm::vec3(0.0f, 0.0, 1.0f));

    glm::mat4 matTranslate = glm::translate(glm::mat4(1.0f),
        glm::vec3(pCurrentMesh->XYZLocation.x,
            pCurrentMesh->XYZLocation.y,
            pCurrentMesh->XYZLocation.z));

    float scale = pCurrentMesh->overallScale;
    glm::mat4 matScale = glm::scale(glm::mat4(1.0f),
        glm::vec3(scale, scale, scale));


    matModel = matModel * matTranslate;
    matModel = matModel * rotateX;
    matModel = matModel * rotateY;
    matModel = matModel * rotateZ;
    matModel = matModel * matScale;


    //            //mat4x4_mul(mvp, p, m);
    //            mvp = matProjection * matView * matModel;
    //
    //    //        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*)mvp);
    //            GLint mvp_location = glGetUniformLocation(shaderProgramNumber, "MVP");
    //
    //            glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(mvp));

                //uniform mat4 mModel;
                //uniform mat4 mView;
                //uniform mat4 mProj;

    GLint mModel_location = glGetUniformLocation(shaderProgramNumber, "mModel");
    GLint mView_location = glGetUniformLocation(shaderProgramNumber, "mView");
    GLint mProj_location = glGetUniformLocation(shaderProgramNumber, "mProj");

    glUniformMatrix4fv(mModel_location, 1, GL_FALSE, glm::value_ptr(matModel));
    glUniformMatrix4fv(mView_location, 1, GL_FALSE, glm::value_ptr(matView));
    glUniformMatrix4fv(mProj_location, 1, GL_FALSE, glm::value_ptr(matProjection));

    // Use the file colours? 
    // uniform bool bUseModelFileColours;
    GLint bUseModelFileColours_UL = glGetUniformLocation(shaderProgramNumber, "bUseModelFileColours");

    if (pCurrentMesh->bUseModelFileColours)
    {
        glUniform1f(bUseModelFileColours_UL, (GLfloat)GL_TRUE);
    }
    else
    {
        glUniform1f(bUseModelFileColours_UL, (GLfloat)GL_FALSE);

        // Get the objectColourRGBA uniform (register) location inside the shader
        GLint objectColourRGBA_UniLoc = glGetUniformLocation(shaderProgramNumber, "objectColourRGBA");

        glUniform4f(objectColourRGBA_UniLoc,
            pCurrentMesh->RGBA.r,
            pCurrentMesh->RGBA.g,
            pCurrentMesh->RGBA.b,
            pCurrentMesh->RGBA.a);
    }//if ( pCurrentMesh->bUseModelFileColours )

    // See if the object is transparent
    // 1.0f then it's solid
    if (pCurrentMesh->RGBA.a < 1.0f)
    {   // It's transparent
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    else
    {   // It's 'solid' (not transparent at all. totally opaque.)
        glDisable(GL_BLEND);
    }



    // Do I want to see the entire object (including the back) 
    // (only makes sense with wireframe object, really)
    if (pCurrentMesh->bDisableDepthBuffer)
    {
        glDisable(GL_DEPTH_TEST);
    }
    else
    {
        glEnable(GL_DEPTH_TEST);
    }

    glCullFace(GL_BACK);

    // Remove "back facing" triangles (just doesn't draw them)
    if (pCurrentMesh->bDisableBackFaceCulling)
    {
        glDisable(GL_CULL_FACE);
    }
    else
    {
        glEnable(GL_CULL_FACE);
    }


    // Change the polygon mode (i.e. how the triangles are filled in)
    // GL_POINT, GL_LINE, and GL_FILL
    if (pCurrentMesh->bIsWireframe)
    {
        // Draw only the lines...
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        // "Fill" in the triangles ("solid" colour)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    // Is this object lit or not
//    uniform bool bDisableLighting;
    GLint bDisableLighting_UL = glGetUniformLocation(shaderProgramNumber, "bDisableLighting");
    if (pCurrentMesh->bIsNotLit)
    {
        glUniform1f(bDisableLighting_UL, (GLfloat)GL_TRUE);
    }
    else
    {
        glUniform1f(bDisableLighting_UL, (GLfloat)GL_FALSE);
    }


    // GL_LINE_LOOP, GL_POINTS, or GL_TRIANGLES
    //        glDrawArrays(GL_TRIANGLES, 0, numberOfCowVerticesToDraw);
    //        glDrawArrays(GL_TRIANGLES, 0, 3);

    sModelDrawInfo modelDrawingInfo;

    if (pVAOManager->FindDrawInfoByModelName(pCurrentMesh->meshFileName, modelDrawingInfo))
    {
        // enable VAO(and everything else)
        glBindVertexArray(modelDrawingInfo.VAO_ID);
        glDrawElements(GL_TRIANGLES, modelDrawingInfo.numberOfIndices, GL_UNSIGNED_INT, (void*)0);
        //disable VAO(and everything else)
        glBindVertexArray(0);
    }

    return;
}// void DrawObject()
