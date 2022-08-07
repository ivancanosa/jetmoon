#include "EditorComponentPanel.hpp"
#include <ctype.h>                            // for tolower
#include <stddef.h>                           // for size_t
#include <iosfwd>                             // for string
#include <string>                             // for operator==, hash
#include <string_view>                        // for operator==
#include <unordered_set>                      // for unordered_set, unordere...
#include <variant>                            // for visit, variant_size_v
#include "ComponentVariant.hpp"               // for ComponentVariant
#include "components/PrefabComponent.hpp"     // for PrefabComponent
#include "core/World.hpp"                     // for World
#include "gameEngineGUI/imguiComponents.hpp"  // for imguiComponent
#include "imgui/imgui.h"                      // for Separator, Text, Begin
#include "utils/type_name.hpp"                // for type_name
#include "utils/variant_by_index.hpp"         // for variant_by_index
struct ServiceContext;


void EditorComponentPanel::render(World* world, ServiceContext* serviceContext, Entity entity){
	variant_by_index<ComponentVariant> type_indexer;
	std::size_t variantCount = std::variant_size_v<ComponentVariant>;
	static char searchComponent[256];
	ImGui::Begin("EditorComponentPanel");
	if(entity != NullEntity){
		bool hasPrefab{false};
		std::unordered_set<std::string>* setPtr{nullptr};
		if(world->hasComponent<PrefabComponent>(entity)){
			hasPrefab = true;
			auto& component = world->getComponent<PrefabComponent>(entity);
			setPtr = &component.changedComponents;
		}

		ImGui::Text("UUID: ");
		ImGui::SameLine();
		ImGui::Text(std::to_string(world->getUUID(entity)).c_str());

		//Add component
		if(ImGui::Button("Add Component...")){
			ImGui::OpenPopup("AddComponentPopup");
		}
		if(ImGui::BeginPopup("AddComponentPopup")){
			ImGui::Text("Component");
			ImGui::Separator();
			ImGui::InputText("##ComponentSearch",  searchComponent, 256);
			std::string strToMatch{searchComponent};
			std::transform(strToMatch.begin(), strToMatch.end(),
				strToMatch.begin(), ::tolower);
			ImGui::Separator();
			for (std::size_t i=0; i < variantCount; i++){
				std::visit([=](auto arg){
					if constexpr (type_name<decltype(arg)>() == "PrefabComponent") return;
					if(!world->hasComponent<decltype(arg)>(entity)){
						std::string typeName{type_name<decltype(arg)>()};
						std::string strToCompare = typeName;
						std::transform(strToCompare.begin(), strToCompare.end(),
							strToCompare.begin(), ::tolower);
						if(strToCompare.find(strToMatch) != std::string::npos){
							if (ImGui::Selectable(typeName.c_str())){
								if(hasPrefab){
									setPtr->insert(std::string(type_name<decltype(arg)>()));
								}
								world->addComponent(entity, arg);
							}
						}
					}
				}, type_indexer.make_default(i));
			}
			ImGui::EndPopup();
		}

		ImGui::Separator();
		//Show components of the entity
		for (std::size_t i=0; i < variantCount; i++){
			std::visit([&](auto arg){
				if(world->hasComponent<decltype(arg)>(entity)){
					if constexpr (type_name<decltype(arg)>() == "PrefabComponent") return;
					if(hasPrefab){
						if(setPtr->find(std::string(type_name<decltype(arg)>())) != setPtr->end()){
							ImGui::Text("(modified)");
							ImGui::SameLine();
						}
					}
					auto& component = world->getComponent<decltype(arg)>(entity);
					bool changed = imguiComponent(world, serviceContext, component, entity);
					if(changed && hasPrefab){
						setPtr->insert(std::string(type_name<decltype(arg)>()));
					}
				}
			}, type_indexer.make_default(i));
		}
	}
	ImGui::End();
}
