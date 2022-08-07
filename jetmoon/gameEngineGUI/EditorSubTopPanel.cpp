#include "EditorSubTopPanel.hpp"
#include <memory>
#include <iosfwd>                       // for string
#include <string>                       // for basic_string, operator==, ope...
#include <type_traits>                  // for remove_extent_t
#include <unordered_map>                // for unordered_map
#include <unordered_set>                // for unordered_set
#include <vector>                       // for vector
#include "GL/glew.h"                    // for GLuint
#include "imgui/imgui.h"                
#include "ImGuizmo/ImGuizmo.h"          // for LOCAL, WORLD
#include "core/ServiceContext.hpp"      // for ServiceContext, EditorDS
#include "core/World.hpp"               // for World
#include "gameEngineGUI/Gizmos.hpp"     // for Gizmos, Gizmos::mode
#include "imgui/imgui.h"                // for ImageButton, PopStyleColor
#include "services/ImageLoader.hpp"     // for ImageLoader
#include "services/WorldManagerDS.hpp"  // for WorldManagerDS
#include "services/EditorDS.hpp"  
#include "commonStructures/ConfigDS.hpp"

void EditorSubTopPanel::render(World* world, ServiceContext* serviceContext){
	static std::string actualWorld;	
	static int e=0;

	ImGui::Begin("SubBar");
	if(ImGui::RadioButton("Local##R0", &e, 0)){
		Gizmos::mode = ImGuizmo::LOCAL;
	}
	ImGui::SameLine();
	if(ImGui::RadioButton("World##R1", &e, 1)){
		Gizmos::mode = ImGuizmo::WORLD;
	}
	ImGui::SameLine();

	ImVec2 size = ImGui::GetWindowSize();
	ImGui::SetCursorPosX(size.x/2);
	if(!serviceContext->editorDS->isPlaying){
		GLuint imageID = serviceContext->imageLoader->load("player-play");
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5, 0.5, 0.5, 0.5));
		if(ImGui::ImageButton(reinterpret_cast<ImTextureID>(imageID), { 25, 25 }, { 0, 1 }, { 1, 0 })){
			std::string currentScene = std::filesystem::path(serviceContext->editorDS->editorConfig.lastOpenProyect).stem().string();
			auto playWorld = world->clone();
			for(auto timeline: serviceContext->editorDS->disabledTimelines){
				playWorld->setTimelineState(timeline, true);
			}
			serviceContext->editorDS->isPlaying = true;
			actualWorld = serviceContext->worldManagerDS->actualWorld; 
			serviceContext->worldManagerDS->addWorld.insert( {currentScene, playWorld} );
			serviceContext->worldManagerDS->actualWorld = currentScene;	
			serviceContext->worldManagerDS->loadedWorlds.insert(currentScene); //We force the world as loaded because it is already as a copy	

			{
				namespace fs = std::filesystem;
				std::string dir = "data/scenes";
				assert(fs::is_directory(dir) && "Directory data/scenes does not exists");

				const fs::path pathToShow{dir};

				for(auto iterEntry = fs::recursive_directory_iterator(pathToShow);
						 iterEntry != fs::recursive_directory_iterator(); 
						 ++iterEntry ) {
					const auto filenameStr = iterEntry->path().stem().string();
					std::string extension = iterEntry->path().extension().string();
					if(!iterEntry->is_directory() && filenameStr != currentScene && extension == ".scene"){
						assert((serviceContext->worldManagerDS->addWorld.find(filenameStr) == serviceContext->worldManagerDS->addWorld.end()) && "Scene duplicated");
						serviceContext->worldManagerDS->addWorld.insert({filenameStr, serviceContext->worldManagerDS->createWorldLambda(iterEntry->path().string())});
					}
				}
			}

		}
		ImGui::PopStyleColor();
	}else{
		GLuint imageID = serviceContext->imageLoader->load("player-stop");
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5, 0.5, 0.5, 0.5));
		if(ImGui::ImageButton(reinterpret_cast<ImTextureID>(imageID), { 25, 25 }, { 0, 1 }, { 1, 0 })){
			serviceContext->editorDS->isPlaying = false;		
			for(auto& world: serviceContext->worldManagerDS->legalWorlds){
				if(world != "editor"){
					serviceContext->worldManagerDS->deleteWorlds.emplace_back(world);	
				}
			}
			serviceContext->worldManagerDS->actualWorld = actualWorld;
		}
		ImGui::PopStyleColor();
	}
	ImGui::End();
}
