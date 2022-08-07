#include "EditorTopPanel.hpp"
#include "services/EditorDS.hpp"             
#include <memory>
#include <cstdlib>                               // for exit
#include <exception>                             // for exception
#include <iostream>                              // for string, operator<<
#include <string>                                // for basic_string<>::valu...
#include <type_traits>                           // for remove_extent_t
#include <vector>                                // for vector
#include "commonStructures/EditorConfig.hpp"     // for EditorConfig
#include "commonStructures/Navmesh.hpp"          // for Navmesh
#include "core/SceneSerializer.hpp"              // for SceneSerializer
#include "core/ServiceContext.hpp"               // for ServiceContext, Edit...
#include "core/World.hpp"                        // for World
#include "core/WorldContext.hpp"                 // for WorldContext, b2WorldDS
#include "gameEngineGUI/DialogStructEditor.hpp"  // for DialogStructEditor
#include "gameEngineGUI/imguiComponents.hpp"     // for fileExplorerDialogLoad
#include "imgui/imgui.h"                         // for MenuItem, BeginMenu
#include "services/EventService.hpp"             // for Key, EventService
#include "GLFW/glfw3.h"                          // for GLFW_KEY_S, GLFW_KEY_F4

void EditorTopPanel::updateOpenProyectsVector(){
	editorConfig->lastOpenProyect = scenePath;
	auto itr = std::find(editorConfig->lastOpenProyects.begin(), editorConfig->lastOpenProyects.end(), scenePath);
	if(itr != editorConfig->lastOpenProyects.end()){
		editorConfig->lastOpenProyects.erase(itr);
	}
	editorConfig->lastOpenProyects.insert(editorConfig->lastOpenProyects.begin(), scenePath);
	if(editorConfig->lastOpenProyects.size() >= 5){
		editorConfig->lastOpenProyects.pop_back();
	}
	editorConfig->save();
}

void EditorTopPanel::newAsDialog(World* world, ServiceContext* serviceContext){
	bool cancel = false;
	if(newDialogCond){
		isDialogOpen = true;
		std::string selectedFileOutput;
		if(fileExplorerDialogLoad("New Scene", selectedFileOutput, cancel, serviceContext, true, ".scene")){
			if(!cancel){
				saveCurrentWorld(world);
				scenePath = selectedFileOutput;
				world->destroyAllEntities();
				resetScene = true;
				updateOpenProyectsVector();
			}
			newDialogCond = false;
			isDialogOpen = false;
		}
		stopwatch.start();
	}
}

void EditorTopPanel::openAsDialog(World* world, ServiceContext* serviceContext){
	bool cancel = false;
	if(openAsDialogCond){
		isDialogOpen = true;
		std::string selectedFileOutput;
		if(fileExplorerDialogLoad("Open Scene", selectedFileOutput, cancel, serviceContext, false, ".scene")){
			if(!cancel){
				auto auxPath = scenePath;
				try{
					saveCurrentWorld(world);
					SceneSerializer serializer;
					scenePath = selectedFileOutput;
					world->destroyAllEntities();
					serializer.loadWorld(world, scenePath);
					resetScene = true;
					updateOpenProyectsVector();

				}catch(std::exception e){
					std::cout << "Error loading the world\n";
					scenePath = auxPath;
				}
			}
			openAsDialogCond = false;
			isDialogOpen = false;
			stopwatch.start();
		}
	}
}

void EditorTopPanel::saveAsDialog(World* world, ServiceContext* serviceContext){
	bool cancel = false;
	if(saveAsDialogCond){
		isDialogOpen = true;
		std::string selectedFileOutput;
		if(fileExplorerDialogLoad("Save Scene as...", selectedFileOutput, cancel, serviceContext, true, ".scene")){
			if(!cancel){
				SceneSerializer serializer;
				scenePath = selectedFileOutput;
				serializer.saveWorld(world, scenePath);
				updateOpenProyectsVector();
			}
			saveAsDialogCond = false;
			isDialogOpen = false;
			stopwatch.start();
		}
	}
}

void EditorTopPanel::editPreferencesDialog(World* world, ServiceContext* serviceContext){
	static std::shared_ptr<DialogStructEditor<EditorConfig>> editorDialog;
	if(editPreferencesCond && !isDialogOpen){
		editorDialog = std::make_shared<DialogStructEditor<EditorConfig>>(*editorConfig);
	}
	if(editPreferencesCond){
		isDialogOpen = true;
		if(editorDialog->update(world, serviceContext)){
			editorDialog = nullptr;
			editPreferencesCond = false;
			isDialogOpen = false;
			editorConfig->save();
		}
	}
}

bool EditorTopPanel::render(World* world, WorldContext* worldContext, ServiceContext* serviceContext, int width, int height){
	resetScene = false;
	static std::string actualWorld;
	bool allowSave{true};
	editorConfig = &serviceContext->editorDS->editorConfig;
	if(scenePath == ""){
		allowSave = false;
	}else{
		if(stopwatch.getTime() >= editorConfig->autosaveTime*1000000 && !serviceContext->editorDS->isPlaying && allowSave && editorConfig->allowAutosave){
			stopwatch.start();
			SceneSerializer serializer;
			serializer.saveWorld(world, scenePath);
			std::cout << "Saved world " << scenePath << std::endl;;
		}
	}
	std::string xStr{"X "}, blank{""}, add{""};

	newAsDialog(world, serviceContext);
	openAsDialog(world, serviceContext);
	saveAsDialog(world, serviceContext);
	editPreferencesDialog(world, serviceContext);

	bool isPlaying = serviceContext->editorDS->isPlaying;
	int lastOpenProyectsCount = editorConfig->lastOpenProyects.size();

	if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
			if(ImGui::MenuItem("New Scene...", "Ctrl+N", false, !isDialogOpen && !isPlaying)){
				newDialogCond = true; // TODO: this must be a non existent file
			}
			if(ImGui::MenuItem("Open Scene", "Ctrl+O", false, !isDialogOpen && !isPlaying)){
				openAsDialogCond = true; // TODO: check if the format is correct
			}
			if (ImGui::BeginMenu("Open Recent...", lastOpenProyectsCount > 0 && !isDialogOpen && !isPlaying)) {
				for(auto path: editorConfig->lastOpenProyects){
					if(ImGui::MenuItem(path.c_str(), "", false, !isDialogOpen)){
						auto auxPath = scenePath;
						try{
							saveCurrentWorld(world);
							SceneSerializer serializer;
							scenePath = path;
							world->destroyAllEntities();
							serializer.loadWorld(world, scenePath);
							resetScene = true;
							updateOpenProyectsVector();
							stopwatch.start();
						}catch(std::exception e){
							std::cout << "Error opening scene " + path << std::endl;
							scenePath = auxPath;
						}
					}
				}
				ImGui::EndMenu();
			}

			ImGui::Separator();
			if(ImGui::MenuItem("Save Scene", "Ctrl+S", false, allowSave && !isDialogOpen && !isPlaying)){
				saveCurrentWorld(world);
			}
			if(ImGui::MenuItem("Save Scene as...", "Ctrl+Shift+S", false, !isDialogOpen && !isPlaying)){
				saveAsDialogCond = true;
			}
			ImGui::Separator();
			if(ImGui::MenuItem("Exit", "Alt+F4")){
				serviceContext->eventService->isExit = true;
			}

			ImGui::EndMenu();
		}
        if (ImGui::BeginMenu("Edit")) {
			if(ImGui::MenuItem("Edit preferences", "", false, !isDialogOpen && !isPlaying)){
				editPreferencesCond = true;
			}
			if(ImGui::MenuItem("Hot reload", "", false, !isDialogOpen && !isPlaying)){
				serviceContext->hotReload();
				serviceContext->editorDS->reload = true;
			}


			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("View")) {
			if(serviceContext->editorDS->renderGrid) add = xStr;
			else add = blank;
			if(ImGui::MenuItem((add + "Grid").c_str(), "", false, true)){
				serviceContext->editorDS->renderGrid = !serviceContext->editorDS->renderGrid;
			}
			if(worldContext->b2World.showColliders) add = xStr;
			else add = blank;
			if(ImGui::MenuItem((add + "Collision Shapes").c_str(), "", false, true)){
				worldContext->b2World.showColliders = !worldContext->b2World.showColliders;
			}
			if(worldContext->navmesh.render) add = xStr;
			else add = blank;
			if(ImGui::MenuItem((add + "Navmesh").c_str(), "", false, true)){
				worldContext->navmesh.render = !worldContext->navmesh.render;
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Help")) {

			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	auto* eventService = EventService::actualEventService;
	for(Key key: eventService->eventVector){
		bool control = key.mods & GLFW_MOD_CONTROL;
		bool shift = key.mods & GLFW_MOD_SHIFT;
		bool alt = key.mods & GLFW_MOD_ALT;
		if(key.action != GLFW_PRESS) continue;

		if(!isDialogOpen && !isPlaying && key.key == GLFW_KEY_O && control){
			openAsDialogCond = true;
		}else if(!isDialogOpen && !isPlaying && key.key == GLFW_KEY_N && control){
			newDialogCond = true;
		}else if(!isDialogOpen && !isPlaying && key.key == GLFW_KEY_S && control && allowSave){
			SceneSerializer serializer;
			serializer.saveWorld(world, scenePath);
		}else if(!isDialogOpen && !isPlaying && key.key == GLFW_KEY_S && control && shift){
			saveAsDialogCond = true;
		}else if(!isDialogOpen && !isPlaying && key.key == GLFW_KEY_F6){
			serviceContext->hotReload();
			serviceContext->editorDS->reload = true;
		}else if(key.key == GLFW_KEY_F4 && alt){
			std::exit(42);
		}
	}
	return resetScene;
}

void EditorTopPanel::saveCurrentWorld(World* world){
	if(scenePath != ""){
		SceneSerializer serializer;
		serializer.saveWorld(world, scenePath);
	}

}
