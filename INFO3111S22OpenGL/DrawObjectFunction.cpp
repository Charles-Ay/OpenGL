// DrawObjectFunction.cpp

#include "globalStuff.h"        // glad and GFLW (i.e. OpenGL call stuff)p
#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> 
// glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

#include "cBasicTextureManager/cBasicTextureManager.h"
#include <iostream>

extern cBasicTextureManager* pTheTexureManager;



void SetUpTextureBindings(GLuint shaderProgramNumber, cMesh* pCurrentMesh);


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


    // Set all the texture information for THIS object
    SetUpTextureBindings(shaderProgramNumber, pCurrentMesh);


    // HACK: See if this is the floor object and if so, apply the discard transparency
    if (pCurrentMesh->friendlyName == "SeaFloor")
    {
        //        uniform bool bUseStencil;
        //        uniform sampler2D stencilTexture01;

        GLint bUseStencil_UL = glGetUniformLocation(shaderProgramNumber, "bUseStencil");
        glUniform1f(bUseStencil_UL, (GLfloat)GL_TRUE);

        // There are AT LEAST 80 (eighty) texture units.
        // We have to be careful we don't assign more than one texture to one unit at any one time
        {
            // Texture binding...
            GLuint textureUnit = 20;			// Texture unit go from 0 to 79
            // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glActiveTexture.xhtml
            glActiveTexture(textureUnit + GL_TEXTURE0);	// GL_TEXTURE0 = 33984

            // Setup the texture bindings
            GLuint stencilTextureID = ::pTheTexureManager->getTextureIDFromName("cat.bmp");
            glBindTexture(GL_TEXTURE_2D, stencilTextureID);

            // glBindTextureUnit( texture00Unit, texture00Number );	// OpenGL 4.5+ only
            // Set texture unit in the shader, too
            GLint stencilSamplerID = glGetUniformLocation(shaderProgramNumber, "stencilTexture01");
            glUniform1i(stencilSamplerID, textureUnit);
        }

    }
    else
    {
        GLint bUseStencil_UL = glGetUniformLocation(shaderProgramNumber, "bUseStencil");
        glUniform1f(bUseStencil_UL, (GLfloat)GL_FALSE);
    }

    // HACK: Apply the height map texture 
    if (pCurrentMesh->friendlyName == "HeightMapObject")
    {
        //        uniform bool bUseHeightMap;
        //        uniform sampler2D heightMapTexture;

        GLint bUseHeightMap_UL = glGetUniformLocation(shaderProgramNumber, "bUseHeightMap");
        glUniform1f(bUseHeightMap_UL, (GLfloat)GL_TRUE);

        // There are AT LEAST 80 (eighty) texture units.
        // We have to be careful we don't assign more than one texture to one unit at any one time
        {
            // Texture binding...
            GLuint textureUnit = 25;			// Texture unit go from 0 to 79
            // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glActiveTexture.xhtml
            glActiveTexture(textureUnit + GL_TEXTURE0);	// GL_TEXTURE0 = 33984

            // Setup the texture bindings
            GLuint stencilTextureID = ::pTheTexureManager->getTextureIDFromName("iceland_heightmap.bmp");
            glBindTexture(GL_TEXTURE_2D, stencilTextureID);

            // glBindTextureUnit( texture00Unit, texture00Number );	// OpenGL 4.5+ only
            // Set texture unit in the shader, too
            GLint heightMapTextureID = glGetUniformLocation(shaderProgramNumber, "heightMapTexture");
            glUniform1i(heightMapTextureID, textureUnit);
        }

    }
    else
    {
        GLint bUseHeightMap_UL = glGetUniformLocation(shaderProgramNumber, "bUseHeightMap");
        glUniform1f(bUseHeightMap_UL, (GLfloat)GL_FALSE);
    }

	



    // HACK: Apply colour to palm trees
    if (pCurrentMesh->friendlyName.find("Palm") != std::string::npos)
    {
        //        uniform bool bUseHeightMap;
        //        uniform sampler2D heightMapTexture;

        GLint bUseModelFileColours_UL = glGetUniformLocation(shaderProgramNumber, "bUseModelFileColours");
        glUniform1f(bUseModelFileColours_UL, (GLfloat)GL_TRUE);

    }
    else
    {
        GLint bUseModelFileColours_UL = glGetUniformLocation(shaderProgramNumber, "bUseModelFileColours");
        glUniform1f(bUseModelFileColours_UL, (GLfloat)GL_FALSE);
    }


    // HACK: Apply colour to palm trees
    if (pCurrentMesh->friendlyName.find("Palm") != std::string::npos)
    {
        //        uniform bool bUseHeightMap;
        //        uniform sampler2D heightMapTexture;

        GLint bDontUseTextureColour_UL = glGetUniformLocation(shaderProgramNumber, "bDontUseTextureColour");
        glUniform1f(bDontUseTextureColour_UL, (GLfloat)GL_TRUE);

    }
    else
    {
        GLint bDontUseTextureColour_UL = glGetUniformLocation(shaderProgramNumber, "bDontUseTextureColour");
        glUniform1f(bDontUseTextureColour_UL, (GLfloat)GL_FALSE);
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




void SetUpTextureBindings(GLuint shaderProgramNumber, cMesh* pCurrentMesh)
{
    {
        // Texture binding...
        GLuint texture00Unit = 0;			// Texture unit go from 0 to 79
        // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glActiveTexture.xhtml
        glActiveTexture(texture00Unit + GL_TEXTURE0);	// GL_TEXTURE0 = 33984

        // Setup the texture bindings
    //    GLuint texture00Number = pTheTexureManager->getTextureIDFromName("WaterSurfaceTexture.bmp");
        GLuint texture00Number = ::pTheTexureManager->getTextureIDFromName(pCurrentMesh->textures[0]);
        glBindTexture(GL_TEXTURE_2D, texture00Number);

        // glBindTextureUnit( texture00Unit, texture00Number );	// OpenGL 4.5+ only
        // Set texture unit in the shader, too
        GLint texture01_UL = glGetUniformLocation(shaderProgramNumber, "texture01");
        glUniform1i(texture01_UL, texture00Unit);
    }


    {
        // Texture binding...
        GLuint texture01Unit = 1;			// Texture unit go from 0 to 79
        // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glActiveTexture.xhtml
        glActiveTexture(texture01Unit + GL_TEXTURE0);	// GL_TEXTURE0 = 33984

        // Setup the texture bindings
    //    GLuint texture00Number = pTheTexureManager->getTextureIDFromName("WaterSurfaceTexture.bmp");
        GLuint texture01Number = ::pTheTexureManager->getTextureIDFromName(pCurrentMesh->textures[1]);
        glBindTexture(GL_TEXTURE_2D, texture01Number);

        // glBindTextureUnit( texture00Unit, texture00Number );	// OpenGL 4.5+ only
        // Set texture unit in the shader, too
        GLint texture02_UL = glGetUniformLocation(shaderProgramNumber, "texture02");
        glUniform1i(texture02_UL, texture01Unit);
    }

    {
        // Texture binding...
        GLuint texture02Unit = 2;			// Texture unit go from 0 to 79
        // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glActiveTexture.xhtml
        glActiveTexture(texture02Unit + GL_TEXTURE0);	// GL_TEXTURE0 = 33984

        // Setup the texture bindings
    //    GLuint texture00Number = pTheTexureManager->getTextureIDFromName("WaterSurfaceTexture.bmp");
        GLuint texture02Number = ::pTheTexureManager->getTextureIDFromName(pCurrentMesh->textures[2]);
        glBindTexture(GL_TEXTURE_2D, texture02Number);

        // glBindTextureUnit( texture00Unit, texture00Number );	// OpenGL 4.5+ only
        // Set texture unit in the shader, too
        GLint texture03_UL = glGetUniformLocation(shaderProgramNumber, "texture03");
        glUniform1i(texture03_UL, texture02Unit);
    }

    {
        // Texture binding...
        GLuint texture03Unit = 3;			// Texture unit go from 0 to 79
        // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glActiveTexture.xhtml
        glActiveTexture(texture03Unit + GL_TEXTURE0);	// GL_TEXTURE0 = 33984

        // Setup the texture bindings
    //    GLuint texture00Number = pTheTexureManager->getTextureIDFromName("WaterSurfaceTexture.bmp");
        GLuint texture03Number = ::pTheTexureManager->getTextureIDFromName(pCurrentMesh->textures[3]);
        glBindTexture(GL_TEXTURE_2D, texture03Number);

        // glBindTextureUnit( texture00Unit, texture00Number );	// OpenGL 4.5+ only
        // Set texture unit in the shader, too
        GLint texture04_UL = glGetUniformLocation(shaderProgramNumber, "texture04");
        glUniform1i(texture04_UL, texture03Unit);
    }

    // Now copy over the texture mixing ratios
    // uniform vec4 texture01to04Ratio;
    GLint texture01to04Ratio_UL = glGetUniformLocation(shaderProgramNumber, "texture01to04Ratio");

    // Take the values from the cMesh.float textureRatio[MAXNUMTEXTURES] variables
    glUniform4f(texture01to04Ratio_UL,
        pCurrentMesh->textureRatio[0],
        pCurrentMesh->textureRatio[1],
        pCurrentMesh->textureRatio[2],
        pCurrentMesh->textureRatio[3]);

    return;
}
