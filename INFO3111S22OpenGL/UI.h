#pragma once
#include <cstddef>
#include "include/glm/glm.hpp"
#include "cLightManager.h"

class UI
{
public:
	UI();
	~UI();
	struct LightValueChanges {
		bool on = true;
		bool lightChange = false;
		float lightType = 0;
		bool posChange = false;
		bool difChange = false;
		bool attenChange = false;
		bool innerOuterChange = false;

		glm::vec4 pos = glm::vec4(0.0f);
		glm::vec4 dif = glm::vec4(0.0f);
		glm::vec4 atten = glm::vec4(0.0f);
		glm::vec4 innerOuter = glm::vec4(0.0f);
		//TODO make operator for comparison
		inline bool operator != (const sLightDescription light) const {
			if (light.position != pos) {
				return true;
			}
			if (light.param2.x != static_cast<float>(on)) {
				return true;
			}
			if(light.diffuse != dif) {
				return true;
			}
			if(light.atten != atten) {
				return true;
			}
			if(light.param1 != innerOuter) {
				return true;
			}
			return false;
		}

		inline bool operator == (const sLightDescription light) const {
			if (light.position != pos) {
				return false;
			}
			if (light.param2.x != static_cast<float>(on)) {
				return false;
			}
			if (light.diffuse != dif) {
				return false;
			}
			if (light.atten != atten) {
				return false;
			}
			if (light.param1 != innerOuter) {
				return false;
			}
			return true;
		}
		inline bool operator = (const sLightDescription light) {
			pos = light.position;
			on = light.param2.x;
			dif = light.diffuse;
			atten = light.atten;
			innerOuter = light.param1;
			lightType = innerOuter.x;
			return true;
		}
		

	private:
		inline void reset() {
			on = true;
			lightChange = false;
			lightType = 0;
			posChange = false;
			difChange = false;
			attenChange = false;
			innerOuterChange = false;
		}
	};

	struct ModelValueChanges {
		bool posChange = false;
		bool rotChange = false;
		bool scaleChange = false;
		bool colorChange = false;
		bool transChange = false;

		glm::vec3 pos = glm::vec3(0.0f);
		glm::vec3 rot = glm::vec3(0.0f);
		glm::vec4 color = glm::vec4(0.0f);
		float scale = 0.0f;
		float transparancy = 1.0f;
	};
	
	static void ShowMainWindow(bool* p_open = NULL);
	static void InitChanges();
	
private:
	static void ShowSimpleOverlay(bool* p_open);
	static void ShowPropertyEditor(bool* p_open);
	static void ShowMenuFile();
	
	static void HelpMarker(const char* desc);
	static void UpdateModelValueChanges();
	static void UpdateLightValueChanges();
};
