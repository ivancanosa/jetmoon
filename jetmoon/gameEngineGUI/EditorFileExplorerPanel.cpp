#include "EditorFileExplorerPanel.hpp"
#include <tuple>
#include <iosfwd>                                  // for string
#include <string>                                  // for allocator, operator+
#include <unordered_map>                           // for unordered_map, ope...
#include "GL/glew.h"                               // for GLuint
#include "components/NameComponent.hpp"            // for NameComponent
#include "components/PrefabComponent.hpp"          // for PrefabComponent
#include "core/SceneSerializer.hpp"                // for SceneSerializer
#include "core/ServiceContext.hpp"                 // for ServiceContext
#include "core/World.hpp"                          // for World
#include "core/definitions.hpp"                    // for Entity
#include "gameEngineGUI/FileExplorer.hpp"          // for FileExplorer
#include "gameEngineGUI/imguiPayloadDefinition.h"  // for extensionToPayload
#include "imgui/imgui.h"                           // for Columns, NextColumn
#include "services/ImageLoader.hpp"                // for ImageLoader


void EditorFileExplorerPanel::render(World* world, ServiceContext* service){
	float cellSize = thumbnailSize + padding;
	static FileExplorer fileExplorer{};
	float panelWidth = ImGui::GetContentRegionAvail().x;

	ImGui::Begin("File Explorer");

	ImGui::Columns(2, "FileExplorerColumns", true);
	currentDirectory = fileExplorer.update();
	ImGui::NextColumn();

	ImGui::BeginChild("File Explorer Child");
	panelWidth = ImGui::GetColumnWidth(1);
	int columnCount = (int)((panelWidth) / cellSize);
	if (columnCount < 1)
		columnCount = 1;
	ImGui::Columns(columnCount, 0, false);

	for (auto& directoryEntry : std::filesystem::directory_iterator(currentDirectory)){
		const auto& path = directoryEntry.path();
		auto relativePath = std::filesystem::relative(path, dataPath);
		std::string filenameString = relativePath.stem().string();
		if(directoryEntry.is_directory()) continue;
		std::string iconName = "file";
		std::string filenameWithoutExtension = relativePath.stem().string();
		std::string extension = directoryEntry.path().extension().u8string();
		if(service->imageLoader->isLoaded(filenameWithoutExtension)){
			iconName = filenameWithoutExtension;
		}
		ImGui::PushID(filenameString.c_str());
		GLuint imageID = service->imageLoader->load(iconName);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		ImGui::ImageButton(reinterpret_cast<ImTextureID>(imageID), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

		if(extensionToPayload.find(extension) != extensionToPayload.end()){
			if (ImGui::BeginDragDropSource()) {
				std::string itemPath = currentDirectory.string() + "/" + filenameString + extension;
				ImGui::SetDragDropPayload(extensionToPayload.find(extension)->second.c_str(), itemPath.c_str(), itemPath.size()+1);
				ImGui::Text(extensionToPayload.find(extension)->second.c_str());
				ImGui::EndDragDropSource();
			}
		}
		ImGui::PopStyleColor();
		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
			if (directoryEntry.is_directory())
				currentDirectory /= path.filename();

		}
		ImGui::TextWrapped("%s", filenameString.c_str());
		ImGui::NextColumn();
		ImGui::PopID();
	}

	ImGui::EndChild();
	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(entityPayload.c_str())){
			IM_ASSERT(payload->DataSize == sizeof(Entity));
			Entity payload_n = *(const Entity*)payload->Data;
			std::string name = std::to_string(payload_n);
			if(world->hasComponent<NameComponent>(payload_n))
				name = world->getComponent<NameComponent>(payload_n).name;
			std::string prefabDir = currentDirectory.string() + "/" + name + ".prefab";
			if(!world->hasComponent<PrefabComponent>(payload_n)){
				PrefabComponent prefabComponent;
				prefabComponent.prefab = prefabDir;
				prefabComponent.load = false;
				world->addComponent<PrefabComponent>(payload_n, prefabComponent);
			}else{
				prefabDir = world->getComponent<PrefabComponent>(payload_n).prefab;
			}
			SceneSerializer serializer;
			serializer.savePrefab(world, payload_n, prefabDir);
		}
		ImGui::EndDragDropTarget();
	}

	ImGui::End();
}
