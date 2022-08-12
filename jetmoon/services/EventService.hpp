#pragma once

//#include "services/SteamService.hpp"

#include <cstdlib>                   // for abs
#include <vector>
#include <array>
#include <GLFW/glfw3.h>
#include <assert.h>
//#include "utils/nuklear.h"
//#include "utils/nuklear_glfw_gl4.h"
#include "imgui/imgui_impl_glfw.h"     // for ImGui_ImplGlfw_NewFrame
#include "core/definitions.hpp"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);


enum InputEnum{
	// Keyboard
	Q, W, E, R, T, Y, U, I, O, P, A, S, D, F,
	G, H, J, K, L, Z, X, C, V, B, N, M, Space, Enter, Tab,

	// Mouse
	Left_Click, Right_Click, Middle_Click, Mouse_X, Mouse_Y, Mouse_Wheel,


	// Gamepad
	GAMEPAD_A, GAMEPAD_B, GAMEPAD_X, GAMEPAD_Y,
	GAMEPAD_UP, GAMEPAD_RIGHT, GAMEPAD_DOWN, GAMEPAD_LEFT,
	GAMEPAD_LEFT_BUMPER, GAMEPAD_RIGHT_BUMPER, GAMEPAD_LEFT_THUMB, GAMEPAD_RIGHT_THUMB,
	GAMEPAD_BACK, GAMEPAD_START,  GAMEPAD_GUIDE,
	GAMEPAD_LEFT_AXIS_X, GAMEPAD_LEFT_AXIS_Y, GAMEPAD_RIGHT_AXIS_X, GAMEPAD_RIGHT_AXIS_Y, 
	GAMEPAD_LEFT_TRIGGER, GAMEPAD_RIGHT_TRIGGER
};

enum Device{
	KEYBOARD_MOUSE, GAMEPAD
};

bool isKeyboard(unsigned int input);
bool isMouseInput(unsigned int input);
bool isMouseFloatInput(unsigned int input);
bool isGamepadInput(unsigned int input);
bool isGamepadBoolInput(unsigned int input);
bool isGamepadFloatInput(unsigned int input);

int inputEnumToGlfwKeyboard(unsigned int input);
int inputEnumToGlfwMouseKey(unsigned int input);
int inputEnumToGlfwBoolGamepad(unsigned int input);
int inputEnumToGlfwFloatGamepad(unsigned int input);

struct MappingCell{
	int mappingCount{0};
	std::array<int, 16> map{};

	void addMap(int value){
		map[mappingCount] = value;
		mappingCount += 1;
	}
};

struct SemanticInput{

	void init(){
	}

	void reset(){
		for(int i=0; i<128; i++){
			semanticToPhysicalMap[i].mappingCount = 0;
			physicalToSemanticMap[i].mappingCount = 0;
		}
	}

	void mapBool(int id, InputEnum physical){
		semanticToPhysicalMap[id].addMap(physical);
		physicalToSemanticMap[physical].addMap(id);
		for(int i=0; i<numberControllers; i++){
		}
	}

	void mapFloat(int id, InputEnum physical){
		semanticToPhysicalMap[id].addMap(physical);
		physicalToSemanticMap[physical].addMap(id);
	}

	bool isInputDown(int id){
		for(int i=0; i<semanticToPhysicalMap[id].mappingCount; i++){
			unsigned int input = semanticToPhysicalMap[id].map[i];
			if(isKeyboard(input)){
				unsigned int glfwKey = inputEnumToGlfwKeyboard(input);
				if(GLFW_PRESS == glfwGetKey(window, glfwKey))
					return true;
			}else if(isMouseInput(input)){
				unsigned int glfwKey = inputEnumToGlfwMouseKey(input);
				if(GLFW_PRESS == glfwGetMouseButton(window, glfwKey))
					return true;
			}else if(existsGamepad){
				if(!isGamepadFloatInput(input)){
					unsigned int glfwKey = gamepadState.buttons[inputEnumToGlfwBoolGamepad(input)];
					if(glfwKey == GLFW_PRESS)
						return true;
				}else{
					float value = gamepadState.axes[inputEnumToGlfwFloatGamepad(input)];
					if(std::abs(value) >= 0.1)
						return true;
				}
			}
		}
		return false;
	}

	bool isInputUp(int id){
		for(int i=0; i<semanticToPhysicalMap[id].mappingCount; i++){
			unsigned int input = semanticToPhysicalMap[id].map[i];
			if(isKeyboard(input)){
				unsigned int glfwKey = inputEnumToGlfwKeyboard(input);
				if(GLFW_RELEASE == glfwGetKey(window, glfwKey))
					return true;
			}else if(isMouseInput(input)){
				unsigned int glfwKey = inputEnumToGlfwMouseKey(input);
				if(GLFW_RELEASE == glfwGetMouseButton(window, glfwKey))
					return true;
			}else if(existsGamepad){
				if(!isGamepadFloatInput(input)){
					unsigned int glfwKey = gamepadState.buttons[inputEnumToGlfwBoolGamepad(input)];
					if(glfwKey == GLFW_RELEASE)
						return true;
				}else{
					float value = gamepadState.axes[inputEnumToGlfwFloatGamepad(input)];
					if(std::abs(value) < 0.1)
						return true;
				}
			}
		}
		return false;
	}

	bool isInputDownOnce(int id){
		bool trueInput = isInputDown(id);
		bool canGetInput = isInputDownOnceMap[id];
		bool returnValue = trueInput && canGetInput;
		isInputDownOnceMap[id] = (!trueInput && canGetInput) || (!trueInput && !canGetInput);
		return returnValue;
	}

	bool isInputUpOnce(int id){
		bool trueInput = isInputUp(id);
		bool canGetInput = isInputUpOnceMap[id];
		bool returnValue = trueInput && canGetInput;
		isInputUpOnceMap[id] = (!trueInput && canGetInput) || (!trueInput && !canGetInput);
		return returnValue;
	}

	float getInputFloat(int id){
		double value = 0.;
		double _;
		for(int i=0; i<semanticToPhysicalMap[id].mappingCount; i++){
			unsigned int input = semanticToPhysicalMap[id].map[i];
			if(isMouseFloatInput(input)){
				if(input == Mouse_X){
					glfwGetCursorPos(window, &value, &_);
					value = std::clamp<float>((value - imgui_viewport[0]) * imgui_viewport[2], 0, 9000.);
					break;
				}else if(input == Mouse_Y){
					glfwGetCursorPos(window, &_, &value);
					value = std::clamp<float>((value - imgui_viewport[1]) * imgui_viewport[3], 0, 9000.);
					break;
				}else if(input == Mouse_Wheel){
					assert(false && "Mouse wheel not suported");
				}
			}else if(existsGamepad && isGamepadFloatInput(input)){
				value = gamepadState.axes[inputEnumToGlfwFloatGamepad(input)];
				break;
			}
		}
		return (float)value;
	}

	std::array<MappingCell, 128> semanticToPhysicalMap{};
	std::array<MappingCell, 128> physicalToSemanticMap{};
	std::array<bool, 128> isInputDownOnceMap{true};
	std::array<bool, 128> isInputUpOnceMap{true};

	GLFWwindow* window;
	GLFWgamepadstate gamepadState;
//	InputHandle_t inputHandles[4];
	bool existsGamepad{false};
	int numberControllers{0};
};

struct Key{
	int key, scancode, action, mods;
};


class EventService{
	public:
		void init(GLFWwindow* window);
		void update();

		std::vector<Key> eventVector;
		bool isExit{false};
		Device lastUsedDevice{Device::KEYBOARD_MOUSE};
		static EventService* actualEventService;
		SemanticInput semanticInput{};
};



