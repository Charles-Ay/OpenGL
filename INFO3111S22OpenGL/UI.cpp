#include "UI.h"
#include "dear/imgui.h"
#include "globalStuff.h"
#include <sstream>
#include <iostream>

extern std::vector< cMesh* > g_vec_pMeshesToDraw;
static std::vector<UI::LightValueChanges> changes_l;
static std::vector<UI::ModelValueChanges> changes_m;


extern cLightManager* pTheLightManager;

extern bool g_EnableDebugLightSpeheres;
extern bool bUseFlyCamera;	// = true

void UI::InitChanges() {
	changes_l = std::vector<LightValueChanges>(pTheLightManager->numberOfUsedLights);
    for (int i = 0; i < pTheLightManager->numberOfUsedLights; ++i) {
        changes_l[i].on = pTheLightManager->theLights[i].param2.x;
        changes_l[i].lightType = pTheLightManager->theLights[i].param1.x;
    }
	
	changes_m = std::vector<ModelValueChanges>(g_vec_pMeshesToDraw.size());
}


void UI::ShowMainWindow(bool* p_open)
{
	IM_ASSERT(ImGui::GetCurrentContext() != NULL && "Missing dear imgui context. Refer to examples app!");
	
	static bool show_app_simple_overlay = false;
	static bool show_app_property_editor = false;

	if (show_app_simple_overlay)      ShowSimpleOverlay(&show_app_simple_overlay);
	if (show_app_property_editor)     ShowPropertyEditor(&show_app_property_editor);
    int b = 12;
	
    // Various window flags. Typically you would just use the default!
    static bool no_titlebar = false;
    static bool no_scrollbar = false;
    static bool no_menu = false;
    static bool no_move = false;
    static bool no_resize = false;
    static bool no_collapse = false;
    static bool no_close = false;
    static bool no_nav = false;
    static bool no_background = false;
    static bool no_bring_to_front = false;

    ImGuiWindowFlags window_flags = 0;
    if (no_titlebar)        window_flags |= ImGuiWindowFlags_NoTitleBar;
    if (no_scrollbar)       window_flags |= ImGuiWindowFlags_NoScrollbar;
    if (!no_menu)           window_flags |= ImGuiWindowFlags_MenuBar;
    if (no_move)            window_flags |= ImGuiWindowFlags_NoMove;
    if (no_resize)          window_flags |= ImGuiWindowFlags_NoResize;
    if (no_collapse)        window_flags |= ImGuiWindowFlags_NoCollapse;
    if (no_nav)             window_flags |= ImGuiWindowFlags_NoNav;
    if (no_background)      window_flags |= ImGuiWindowFlags_NoBackground;
    if (no_bring_to_front)  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
    if (no_close)           p_open = NULL; // Don't pass our bool* to Begin
	
     // We specify a default position/size in case there's no data in the .ini file
    ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);

    // Main body of the window start.
    if (!ImGui::Begin("OpenGL ImGUI", p_open, window_flags))
    {
        // Early out if the window is collapsed, as an optimization.
        ImGui::End();
        return;
    }
	
    // Most "big" widgets share a common width settings by default.
    //ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.65f);             // Use 2/3 of the space for widgets and 1/3 for labels (default)
    ImGui::PushItemWidth(ImGui::GetFontSize() * -12);           // Use fixed width for labels (by passing a negative value), 
                                                                         // the rest goes to widgets. We choose a width proportional to our font size.

        // Menu Bar
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Menu"))
        {
            ShowMenuFile();
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Mesh"))
        {
            ImGui::MenuItem("Property editor", NULL, &show_app_property_editor);
            ImGui::MenuItem("Simple overlay", NULL, &show_app_simple_overlay);
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
	
    ImGui::Text("Charles OpenGL debug menu using Dear ImGui version: (%s)", IMGUI_VERSION);
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Help"))
    {
        ImGui::Text("ABOUT THIS DEMO:");
        ImGui::BulletText("Sections below are demonstrating many aspects of the library.");
        ImGui::BulletText("The \"Examples\" menu above leads to more demo contents.");
        ImGui::BulletText("The \"Tools\" menu above gives access to: About Box, Style Editor,\n"
            "and Metrics (general purpose Dear ImGui debugging tool).");
        ImGui::Separator();

        ImGui::Text("PROGRAMMER GUIDE:");
        ImGui::BulletText("See the ShowDemoWindow() code in imgui_demo.cpp. <- you are here!");
        ImGui::BulletText("See comments in imgui.cpp.");
        ImGui::BulletText("See example applications in the examples/ folder.");
        ImGui::BulletText("Read the FAQ at http://www.dearimgui.org/faq/");
        ImGui::BulletText("Set 'io.ConfigFlags |= NavEnableKeyboard' for keyboard controls.");
        ImGui::BulletText("Set 'io.ConfigFlags |= NavEnableGamepad' for gamepad controls.");
        ImGui::Separator();

        ImGui::Text("USER GUIDE:");
        ImGui::ShowUserGuide();
    }
	
    if (ImGui::CollapsingHeader("Configuration"))
    {
        ImGuiIO& io = ImGui::GetIO();
		
        if (ImGui::TreeNode("Style"))
        {
            HelpMarker("The same contents can be accessed in 'Tools->Style Editor' or by calling the ShowStyleEditor() function.");
            ImGui::ShowStyleEditor();
            ImGui::TreePop();
            ImGui::Separator();
        }
		
        if (ImGui::TreeNode("Debug"))
        {
            HelpMarker("This menu contains items for debugging purposes.\n"
				"Please read the comments in imgui.cpp for details.");
            ImGui::Checkbox("Enable Debug Spheres", &g_EnableDebugLightSpeheres);
            ImGui::Checkbox("Enable Fly Camera", &bUseFlyCamera);
            ImGui::TreePop();
            ImGui::Separator();
        }

        if (ImGui::TreeNode("Capture/Logging"))
        {
            ImGui::TextWrapped("The logging API redirects all text output so you can easily capture the content of a window or a block. Tree nodes can be automatically expanded.");
            HelpMarker("Try opening any of the contents below in this window and then click one of the \"Log To\" button.");
            ImGui::LogButtons();
            ImGui::TextWrapped("You can also call ImGui::LogText() to output directly to the log without a visual output.");
            if (ImGui::Button("Copy \"Hello, world!\" to clipboard"))
            {
                ImGui::LogToClipboard();
                ImGui::LogText("Hello, world!");
                ImGui::LogFinish();
            }
            ImGui::TreePop();
        }
    }
	
    if (ImGui::CollapsingHeader("Window options"))
    {
        ImGui::Checkbox("No titlebar", &no_titlebar); ImGui::SameLine(150);
        ImGui::Checkbox("No scrollbar", &no_scrollbar); ImGui::SameLine(300);
        ImGui::Checkbox("No menu", &no_menu);
        ImGui::Checkbox("No move", &no_move); ImGui::SameLine(150);
        ImGui::Checkbox("No resize", &no_resize); ImGui::SameLine(300);
        ImGui::Checkbox("No collapse", &no_collapse);
        ImGui::Checkbox("No close", &no_close); ImGui::SameLine(150);
        ImGui::Checkbox("No nav", &no_nav); ImGui::SameLine(300);
        ImGui::Checkbox("No background", &no_background);
        ImGui::Checkbox("No bring to front", &no_bring_to_front);
    }
	
    ImGui::End();
}

void UI::ShowSimpleOverlay(bool* p_open)
{
    const float DISTANCE = 10.0f;
    static int corner = 0;
    ImGuiIO& io = ImGui::GetIO();
    if (corner != -1)
    {
        ImVec2 window_pos = ImVec2((corner & 1) ? io.DisplaySize.x - DISTANCE : DISTANCE, (corner & 2) ? io.DisplaySize.y - DISTANCE : DISTANCE);
        ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
    }
    ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
    if (ImGui::Begin("Camera overlay", p_open, (corner != -1 ? ImGuiWindowFlags_NoMove : 0) | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav))
    {
        ImGui::Text("Camaera overlay\n" "in the corner of the screen.\n" "(right-click to change position)");
        ImGui::Separator();
        if (bUseFlyCamera) {
            ImGui::Text("Fly Camera Position: (%.1f, %.1f, %.1f)", g_pFlyCamera->eye.x, g_pFlyCamera->eye.y, g_pFlyCamera->eye.z);
            ImGui::Separator();
            glm::vec3 look = g_pFlyCamera->getAt();
            ImGui::Text("Fly Camera Looking at: (%.1f, %.1f, %.1f)", look.x, look.y, look.z);
        }
        else
            ImGui::Text("Camera Position: (%.1f, %.1f, %.1f)", g_cameraEye.x, g_cameraEye.y, g_cameraEye.z);
        if (ImGui::BeginPopupContextWindow())
        {
            if (ImGui::MenuItem("Custom", NULL, corner == -1)) corner = -1;
            if (ImGui::MenuItem("Top-left", NULL, corner == 0)) corner = 0;
            if (ImGui::MenuItem("Top-right", NULL, corner == 1)) corner = 1;
            if (ImGui::MenuItem("Bottom-left", NULL, corner == 2)) corner = 2;
            if (ImGui::MenuItem("Bottom-right", NULL, corner == 3)) corner = 3;
            if (p_open && ImGui::MenuItem("Close")) *p_open = false;
            ImGui::EndPopup();
        }
    }
    ImGui::End();
}

void UI::ShowPropertyEditor(bool* p_open)
{
    ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Object Property editor", p_open))
    {
        ImGui::End();
        return;
    }
	
    HelpMarker("This is a window used to edit properties of the mesh and lights");
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
    ImGui::Columns(2);
    ImGui::Separator();
	
    struct func
    {
        static void ShowObject(const char* prefix, int uid)
        {
            ImGui::PushID(std::string("obj_id: " + std::to_string(uid)).c_str());                      // Use object uid as identifier. Most commonly you could also use the object pointer as a base ID.
            ImGui::AlignTextToFramePadding();  // Text and Tree nodes are less high than regular widgets, here we add vertical spacing to make the tree lines equal high.
            bool node_open = ImGui::TreeNode("Object", "%s_%u", prefix, uid);
            //ImGui::NextColumn();
            ImGui::AlignTextToFramePadding();
            std::stringstream s;
            s << prefix << std::string(" Objects").c_str();
            ImGui::Text(s.str().c_str());
            if (node_open && uid == 0) {

                for (int i = 0; i < g_vec_pMeshesToDraw.size();++i) {
                    std::stringstream ss;
                    ss << "model" + i;
                    ImGui::PushID(ss.str().c_str());
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                    ImGui::Columns(2);
                    ImGui::Separator();
                    ImGui::Text("ID");
                    ImGui::NextColumn();
                    ImGui::Text(std::to_string(i).c_str());
                    ImGui::NextColumn();
                    ImGui::Separator();
                    ImGui::PopStyleVar();
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
                    ImGui::Columns(2);
                    ImGui::Separator();
					
                    ImGui::Text("Position");
                    ImGui::NextColumn();
                    ImGui::PushItemWidth(-1);
                    float fl3P[3] = { g_vec_pMeshesToDraw[i]->XYZLocation.x, g_vec_pMeshesToDraw[i]->XYZLocation.y, g_vec_pMeshesToDraw[i]->XYZLocation.z };
                    float* ptr = fl3P;
                    changes_m[i].posChange = ImGui::InputFloat3("position", ptr);
					if(changes_m[i].posChange)changes_m[i].pos = glm::vec3(ptr[0], ptr[1], ptr[2]);
                    else changes_m[i].pos = g_vec_pMeshesToDraw[i]->XYZLocation;
                    ImGui::PopItemWidth();
                    ImGui::NextColumn();
                    ImGui::Separator();
					
                    ImGui::Text("Rotation");
                    ImGui::NextColumn();
                    ImGui::PushItemWidth(-1);
                    float fl3R[3] = { g_vec_pMeshesToDraw[i]->orientationEulerAngle.x, g_vec_pMeshesToDraw[i]->orientationEulerAngle.y, g_vec_pMeshesToDraw[i]->orientationEulerAngle.z };
                    ptr = fl3R;
                    changes_m[i].rotChange = ImGui::InputFloat3("rotation", ptr);
					if(changes_m[i].rotChange)changes_m[i].rot = glm::vec3(ptr[0], ptr[1], ptr[2]);
					else changes_m[i].rot = g_vec_pMeshesToDraw[i]->orientationEulerAngle;
                    ImGui::PopItemWidth();
                    ImGui::NextColumn();
                    ImGui::Separator();
					
                    ImGui::Text("Scale");
                    ImGui::NextColumn();
                    ImGui::PushItemWidth(-1);
                    float sc[1] = { g_vec_pMeshesToDraw[i]->overallScale };
                    float* pSc = sc;
                    changes_m[i].scaleChange = ImGui::DragFloat("scale", pSc, 0.01f);
					if(changes_m[i].scaleChange)changes_m[i].scale = *pSc;
					else changes_m[i].scale = g_vec_pMeshesToDraw[i]->overallScale;
                    ImGui::PopItemWidth();
                    ImGui::NextColumn();
                    ImGui::Separator();				

                    ImGui::Text("Color");
                    ImGui::NextColumn();
                    ImGui::PushItemWidth(-1);
                    float fl3C[3] = { g_vec_pMeshesToDraw[i]->RGBA.r, g_vec_pMeshesToDraw[i]->RGBA.g, g_vec_pMeshesToDraw[i]->RGBA.b };
                    ptr = fl3C;
                    changes_m[i].colorChange = ImGui::InputFloat3("Color", ptr);
                    if (changes_m[i].colorChange)changes_m[i].color = glm::vec4(ptr[0], ptr[1], ptr[2], 1.0f);
                    else changes_m[i].color = g_vec_pMeshesToDraw[i]->RGBA;
                    ImGui::PopItemWidth();
                    ImGui::NextColumn();
                    ImGui::Separator();
					
                    ImGui::Text("Transparancy");
                    ImGui::NextColumn();
                    ImGui::PushItemWidth(-1);
                    float tp[1] = { g_vec_pMeshesToDraw[i]->RGBA.a };
                    float* pTp = tp;
                    changes_m[i].transChange = ImGui::DragFloat("transparancy", pTp, 0.01f, 0, 1.0f);
                    if (changes_m[i].transChange) changes_m[i].transparancy = *pTp;
                    else changes_m[i].transparancy = g_vec_pMeshesToDraw[i]->RGBA.a;
                    ImGui::PopItemWidth();
                    ImGui::NextColumn();
                    ImGui::Separator();
					
                    ImGui::Text("Model");
                    ImGui::NextColumn();
                    ImGui::PushItemWidth(-1);
                    ImGui::Text(g_vec_pMeshesToDraw[i]->meshFileName.c_str());
                    ImGui::PopItemWidth();
                    ImGui::NextColumn();
                    ImGui::Separator();
					
                    ImGui::Text("Texture(s)");
                    ImGui::NextColumn();
                    for (int j = 0; j < g_vec_pMeshesToDraw[i]->usedTextures; ++j) {

                        ImGui::PushItemWidth(-1);
                        ImGui::Text(g_vec_pMeshesToDraw[i]->textures[j].c_str());
                        ImGui::PopItemWidth();
						if(i!= g_vec_pMeshesToDraw[i]->usedTextures-1)ImGui::Separator();
                    }
					
                    ImGui::PopStyleVar();
                    ImGui::NextColumn();
                    ImGui::Separator();
                    ImGui::PopID();
                }

                ImGui::TreePop();
            }
            //Lights
            else if(node_open && uid == 1) {
				
                for (int i = 0; i < pTheLightManager->numberOfUsedLights; ++i) {
					//add light attributes to gui
                    std::stringstream ss;
                    ss << "light" + i;
                    ImGui::PushID(ss.str().c_str());
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                    ImGui::Columns(2);
                    ImGui::Separator();
                    ImGui::Text("ID");
                    ImGui::NextColumn();
                    ImGui::Text(std::to_string(i).c_str());
                    ImGui::NextColumn();
                    ImGui::Separator();
                    ImGui::PopStyleVar();
					
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
                    ImGui::Columns(2);
                    ImGui::Separator();

                    ImGui::Text("Is_On");
                    ImGui::NextColumn();
                    ImGui::PushItemWidth(-1);
                    bool old = changes_l[i].on;
                    bool valChange = ImGui::Checkbox("on", &changes_l[i].on);
                    ImGui::PopItemWidth();
                    ImGui::NextColumn();
                    ImGui::Separator();
					
                    ImGui::Text("Look at Target");
                    ImGui::NextColumn();
                    ImGui::PushItemWidth(-1);
                    std::stringstream sp;
                    sp << "current target id: " << pTheLightManager->theLights[i].currentTarget;
                    ImGui::Checkbox(sp.str().c_str(), &pTheLightManager->theLights[i].useTarget);
                    ImGui::PopItemWidth();
                    ImGui::NextColumn();
                    ImGui::Separator();
					
                    ImGui::Text("Next target");
                    ImGui::NextColumn();
                    ImGui::PushItemWidth(-1);
                    bool pressed = false;
                    if (ImGui::Button("go to next target", ImVec2(120, 0))) {
                        pTheLightManager->theLights[i].ChangeTargetToLookAt();
                    }
                    if (pressed)pTheLightManager->theLights[i].ChangeTargetToLookAt();
                    ImGui::PopItemWidth();
                    ImGui::NextColumn();
                    ImGui::Separator();
					
                    ImGui::Text("Light Type");
					ImGui::NextColumn();
                    ImGui::PushItemWidth(-1);				
                    int e = (int)pTheLightManager->theLights[i].param1.x;
                    //std::cout << "Light " << i << ": " << e << std::endl;
                    bool r1 = ImGui::RadioButton("pointlight", &e, 0); ImGui::SameLine();
                    bool r2 = ImGui::RadioButton("spotlight", &e, 1);
					if (r1) {
						//point
                        changes_l[i].lightType = 0;
                        changes_l[i].lightChange = true;
					}
					else if (r2) {
						//spot
                        changes_l[i].lightType = 1;
                        changes_l[i].lightChange = true;
					}
                    else {
                        changes_l[i].lightType = static_cast<float>(e);
                        changes_l[i].lightChange = true;
                    }
                    ImGui::PopItemWidth();
                    ImGui::NextColumn();
                    ImGui::Separator();
					
                    ImGui::Text("Position");
                    ImGui::NextColumn();
                    ImGui::PushItemWidth(-1);
                    float fl3P[3] = { pTheLightManager->theLights[i].position.x , pTheLightManager->theLights[i].position.y, pTheLightManager->theLights[i].position.z };
                    float* ptr = fl3P;
                    changes_l[i].posChange = ImGui::InputFloat3("position", ptr);
                    if (changes_l[i].posChange)changes_l[i].pos = glm::vec4(ptr[0], ptr[1], ptr[2], 1.0f);
                    else changes_l[i].pos = pTheLightManager->theLights[i].position;
                    ImGui::PopItemWidth();
                    ImGui::NextColumn();
                    ImGui::Separator();
					
                    ImGui::Text("Diffuse");
                    ImGui::NextColumn();
                    ImGui::PushItemWidth(-1);
                    float fl3D[3] = { pTheLightManager->theLights[i].diffuse.x, pTheLightManager->theLights[i].diffuse.y, pTheLightManager->theLights[i].diffuse.z };
					ptr = fl3D;
                    changes_l[i].difChange = ImGui::InputFloat3("diffuse", ptr);
					if (changes_l[i].difChange)changes_l[i].dif = glm::vec4(ptr[0], ptr[1], ptr[2], 1.0f);
					else changes_l[i].dif = pTheLightManager->theLights[i].diffuse;
                    ImGui::PopItemWidth();
                    ImGui::NextColumn();
                    ImGui::Separator();
					
                    ImGui::Text("Attenuation");
                    ImGui::NextColumn();
                    ImGui::PushItemWidth(-1);
                    float fl3A[3] = { pTheLightManager->theLights[i].atten.x, pTheLightManager->theLights[i].atten.y, pTheLightManager->theLights[i].atten.z };
					ptr = fl3A;
                    changes_l[i].attenChange = ImGui::DragFloat3("attenuation", ptr, 0.00002f, 0, 0, "%.5f");
					if (changes_l[i].attenChange)changes_l[i].atten = glm::vec4(ptr[0], ptr[1], ptr[2], 1.0f);
                    else changes_l[i].atten = pTheLightManager->theLights[i].atten;
                    ImGui::PopItemWidth();
                    ImGui::NextColumn();
                    ImGui::Separator();
					
                    ImGui::Text("Inner and Outer Angles");
                    ImGui::NextColumn();
                    ImGui::PushItemWidth(-1);
                    float fl2[2] = { pTheLightManager->theLights[i].param1.y, pTheLightManager->theLights[i].param1.z};
					ptr = fl2;
                    changes_l[i].innerOuterChange = ImGui::DragFloat2("angles", ptr, 0.01f);
					if (changes_l[i].innerOuterChange || changes_l[i].lightChange)changes_l[i].innerOuter = glm::vec4(changes_l[i].lightType, ptr[0], ptr[1], 0.0f);
                    else changes_l[i].innerOuter = pTheLightManager->theLights[i].param1;
                    ImGui::PopItemWidth();
                    ImGui::NextColumn();
                    ImGui::Separator();
					
                    ImGui::Text("Material");
                    ImGui::NextColumn();
                    ImGui::PushItemWidth(-1);
                    ImGui::Text(g_vec_pMeshesToDraw[i]->meshFileName.c_str());
                    ImGui::PopItemWidth();
                    ImGui::PopStyleVar();
                    ImGui::NextColumn();
                    ImGui::Separator();
                    ImGui::PopID();
                }

                ImGui::TreePop();
            }
            ImGui::PopID();
        }

    };

    // Iterate dummy objects with dummy members (all the same data)
    for (int obj_i = 0; obj_i < 2; obj_i++) {
        if (obj_i == 0) {
			func::ShowObject("Mesh", obj_i);
		} else {
			func::ShowObject("Light", obj_i);
        }
        UpdateValueChanges();
    }

    ImGui::Columns(1);
    ImGui::Separator();
    ImGui::PopStyleVar();
    ImGui::End();
}

void UI::ShowMenuFile()
{
    std::string errors;
	
    ImGui::MenuItem("File menu", NULL, false, false);
    if (ImGui::MenuItem("Save Models")) {
        SaveSceneToFile(sceneFileName, errors);
    }
    if (ImGui::MenuItem("Save Lights")) {
        pTheLightManager->SaveLightConfigToFile(*pTheLightManager, lightFileName, errors);
    }
    if (ImGui::MenuItem("Save Camera")) {
		SaveCamaeraToFile(cameraFileName);
    }
}

void UI::HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

void UI::UpdateValueChanges() {
    for (int i = 0; i < g_vec_pMeshesToDraw.size(); ++i) {
        glm::vec3 empty = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec4 empty2 = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
        if (changes_m[i].pos != empty)
            g_vec_pMeshesToDraw[i]->XYZLocation = changes_m[i].pos;
        if (changes_m[i].rot != empty)
            g_vec_pMeshesToDraw[i]->orientationEulerAngle = changes_m[i].rot;
        if (changes_m[i].scale != 0.0f)
            g_vec_pMeshesToDraw[i]->overallScale = changes_m[i].scale;
        if (changes_m[i].color != empty2)
            g_vec_pMeshesToDraw[i]->RGBA = glm::vec4(changes_m[i].color.r, changes_m[i].color.g, changes_m[i].color.b, changes_m[i].transparancy);
    }
	
    for (int i = 0; i < pTheLightManager->numberOfUsedLights; ++i) {
		glm::vec4 empty = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
		if(changes_l[i].pos != empty) pTheLightManager->theLights[i].position = changes_l[i].pos;
        if (changes_l[i].dif != empty)pTheLightManager->theLights[i].diffuse = changes_l[i].dif;
        if (changes_l[i].atten != empty)pTheLightManager->theLights[i].atten = changes_l[i].atten;
        if (changes_l[i].innerOuter != empty)pTheLightManager->theLights[i].param1 = changes_l[i].innerOuter;
        pTheLightManager->theLights[i].param2.x = changes_l[i].on;
        pTheLightManager->theLights[i].MoveToTarget();
    }
}