#pragma once
#include <cstddef>
#include "include/glm/glm.hpp"

class UI
{
public:
	UI();
	~UI();
	struct LightValueChanges {
		bool on = true;
		bool lightChange = false;
		int lightType = 0;
		bool posChange = false;
		bool difChange = false;
		bool attenChange = false;
		bool innerOuterChange = false;

		glm::vec4 pos;
		glm::vec4 dif;
		glm::vec4 atten;
		glm::vec4 innerOuter;
	};

	struct ModelValueChanges {
		bool posChange = false;
		bool rotChange = false;
		bool scaleChange = false;
		bool colorChange = false;
		bool transChange = false;

		glm::vec3 pos;
		glm::vec3 rot;
		glm::vec4 color;
		float scale = 0.0f;
		float transparancy;
	};
	
	static void ShowMainWindow(bool* p_open = NULL);
	static void InitChanges();
	
private:
	static void ShowSimpleOverlay(bool* p_open);
	static void ShowPropertyEditor(bool* p_open);
	static void ShowMenuFile();
	
	static void HelpMarker(const char* desc);
	static void UpdateValueChanges();
};
