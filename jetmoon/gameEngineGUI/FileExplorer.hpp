#pragma once

#include <filesystem>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "gameEngineGUI/imguiComponents.hpp"

class FileExplorer{
public:
	FileExplorer(std::string root="data"): root(root){}

	std::string update(){
		int i{0};
		int node_clicked{-1};
		std::string selectedPath = root;

		const std::filesystem::path rootPath = root;
		showDirectoryHierarchyRec(rootPath, node_clicked, i, selectionMask, selectedPath);
		return selectedPath;
	};

private:
	void showDirectoryHierarchyRec(std::filesystem::path currentPath, int& node_clicked, int& i, int& selection_mask, std::string& selectedPath){

	int flags =  ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth;
	int nodeLeafFlag{0};
	for (auto& directoryEntry : std::filesystem::directory_iterator(currentPath)){
		if (!directoryEntry.is_directory()) continue;
		auto& path = directoryEntry.path();
		nodeLeafFlag = ImGuiTreeNodeFlags_Leaf;
		for (auto& directoryEntry2 : std::filesystem::directory_iterator(path)){
			if(directoryEntry2.is_directory()){
				nodeLeafFlag = 0;
				break;
			}
		}
		bool open = fileExplorerDialogDir(directoryEntry, selectedPath, node_clicked, i, selection_mask, flags | nodeLeafFlag);
		if(open){
			showDirectoryHierarchyRec(path, node_clicked, i, selection_mask, selectedPath);
			ImGui::TreePop();
		}
	}
	}

	int selectionMask{0};
	std::string root{};
};
