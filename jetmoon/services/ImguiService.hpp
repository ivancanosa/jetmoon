#pragma once

#include <assert.h>
#include <tuple>
#include <vector>
#include <unordered_map>
#include <map>
#include <string>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"

class ImguiService{
public:
	ImGuiContext* ctx{nullptr};
	ImFontAtlas* fontAtlas{nullptr};

	void setFontRange(int minSize, int maxSize){
		assert(minSize < maxSize && minSize > 5 && "The font size range is incorrect");
		this->minSize = minSize;
		this->maxSize = maxSize;
	}

	void addFontToLoad(std::string id, std::string path){
		fontsToLoadVector.push_back({id, path});
	}

	ImFont* getFont(std::string id, int size){
		size = std::clamp(size, minSize, maxSize);
		return fontsMap[{id, size}];
	}

	void setDefaultFont(std::string id, int size){
		ImGui::SetCurrentContext(ctx);
		size = std::clamp(size, minSize, maxSize);
		ImGui::GetIO().FontDefault = fontsMap[{id, size}];
	}

	void init(GLFWwindow* window){
		const char* glsl_version = "#version 450 core";
		ctx = ImGui::CreateContext();
		ImGui::SetCurrentContext(ctx);
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init(glsl_version);

		for(auto&[id, path]: fontsToLoadVector){
			for(int i=minSize; i<=maxSize; i++){
				fontsMap[{id, i}] = io.Fonts->AddFontFromFileTTF(path.c_str(), i);
			}
		}

		fontAtlas = io.Fonts;
		ImGui_ImplOpenGL3_NewFrame();
	}

private:
	std::vector<std::tuple<std::string, std::string>> fontsToLoadVector{};
	std::map<std::tuple<std::string, int>, ImFont*> fontsMap;
	int minSize{10}, maxSize{40};
};
