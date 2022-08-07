#include "EventService.hpp"
#include <GLFW/glfw3.h>  // for glfwGetGamepadState, glfwPollEvents, glfwSet...
#include <vector>        // for vector


EventService* EventService::actualEventService = nullptr;

bool isKeyboard(unsigned int input){
	return input <= InputEnum::Tab;
}

bool isMouseInput(unsigned int input){
	return input >= InputEnum::Left_Click && input <= InputEnum::Mouse_Wheel;
}

bool isMouseFloatInput(unsigned int input){
	return input >= InputEnum::Mouse_Wheel && input < GAMEPAD_A;
}


bool isGamepadInput(unsigned int input){
	return input > Mouse_Wheel;
}

bool isGamepadBoolInput(unsigned int input){
	return input > Mouse_Wheel && input < GAMEPAD_LEFT_AXIS_X;
}

bool isGamepadFloatInput(unsigned int input){
	return input >= GAMEPAD_LEFT_AXIS_X;
}



int inputEnumToGlfwKeyboard(unsigned int input){
	assert(isKeyboard(input) && "The input is not keyboard");
	const int map[] = {
		GLFW_KEY_Q, GLFW_KEY_W, GLFW_KEY_E, GLFW_KEY_R, GLFW_KEY_T, GLFW_KEY_Y, GLFW_KEY_U, GLFW_KEY_I, GLFW_KEY_O, GLFW_KEY_P,
		GLFW_KEY_A, GLFW_KEY_S, GLFW_KEY_D, GLFW_KEY_F, GLFW_KEY_G, GLFW_KEY_H, GLFW_KEY_J, GLFW_KEY_K, GLFW_KEY_L,
		GLFW_KEY_Z, GLFW_KEY_X,  GLFW_KEY_C,  GLFW_KEY_V,  GLFW_KEY_B,  GLFW_KEY_N,  GLFW_KEY_M, GLFW_KEY_SPACE, GLFW_KEY_ENTER, GLFW_KEY_TAB
	};

	return map[input];
}

int inputEnumToGlfwBoolGamepad(unsigned int input){
	assert(isGamepadBoolInput(input) && "The input is not gamepad bool");
	const int map[] = {
		GLFW_GAMEPAD_BUTTON_A, GLFW_GAMEPAD_BUTTON_B, GLFW_GAMEPAD_BUTTON_X, GLFW_GAMEPAD_BUTTON_Y, 
		GLFW_GAMEPAD_BUTTON_DPAD_UP, GLFW_GAMEPAD_BUTTON_DPAD_RIGHT, GLFW_GAMEPAD_BUTTON_DPAD_DOWN, GLFW_GAMEPAD_BUTTON_DPAD_LEFT,
		GLFW_GAMEPAD_BUTTON_LEFT_BUMPER, GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER, GLFW_GAMEPAD_BUTTON_LEFT_THUMB, GLFW_GAMEPAD_BUTTON_RIGHT_THUMB,
		GLFW_GAMEPAD_BUTTON_BACK, GLFW_GAMEPAD_BUTTON_START, GLFW_GAMEPAD_BUTTON_GUIDE
	};

	return map[input-GAMEPAD_A];
}

int inputEnumToGlfwFloatGamepad(unsigned int input){
	assert(isGamepadFloatInput(input) && "The input is not gamepad float");
	const int map[] = {
		GLFW_GAMEPAD_AXIS_LEFT_X, GLFW_GAMEPAD_AXIS_LEFT_Y, GLFW_GAMEPAD_AXIS_RIGHT_X, GLFW_GAMEPAD_AXIS_RIGHT_Y,
		GLFW_GAMEPAD_AXIS_LEFT_TRIGGER, GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER 
	};

	return map[input-GAMEPAD_LEFT_AXIS_X];
}


int inputEnumToGlfwMouseKey(unsigned int input){
	assert(isMouseInput(input) && "The input is not mouse");
	switch(input){
		case Left_Click:
			return GLFW_MOUSE_BUTTON_LEFT;
		case Right_Click:
			return GLFW_MOUSE_BUTTON_RIGHT;
		case Middle_Click:
			return GLFW_MOUSE_BUTTON_MIDDLE;
		default:
			assert(false && "The input is not mouse");
	}
}
void EventService::init(GLFWwindow* window){
	EventService::actualEventService = this;
	glfwSetKeyCallback(window, key_callback);
	semanticInput.init();
	semanticInput.window = window;
}

void EventService::update(){
	EventService::eventVector.clear();
	glfwPollEvents();
	semanticInput.existsGamepad = glfwGetGamepadState(GLFW_JOYSTICK_1, &semanticInput.gamepadState);
	for(int i = 0; i <= GLFW_GAMEPAD_BUTTON_LAST; i++){
		if(semanticInput.gamepadState.buttons[i] == GLFW_PRESS){
			lastUsedDevice = Device::GAMEPAD;
			break;
		}
	}
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	auto e = EventService::actualEventService;
	Key k{key, scancode, action, mods};
	e->eventVector.emplace_back(k);
	e->lastUsedDevice = Device::KEYBOARD_MOUSE;

	if(glfwWindowShouldClose(window)){
		e->isExit = true;
	}
}
