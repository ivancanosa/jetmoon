#pragma once

#include <vector>
#include <variant>
#include <unordered_set>
#include <filesystem>
#include <iostream>

#include "imgui/imgui.h"

#include "core/definitions.hpp"
#include "core/World.hpp"

#include "utils/variant_by_index.hpp"
#include "utils/type_name.hpp"
#include "utils/visit_struct.hpp"

#include "components/Transform.hpp"
#include "components/Sprite.hpp"
#include "components/ParticleEmisor.hpp"
#include "components/RigidBody2D.hpp"
#include "components/SpriteAnimation.hpp"
#include "components/Trail.hpp"
#include "components/AudioSource.hpp"
#include "components/LightSource.hpp"

#include "configShaderDSVariant.h"

struct ServiceContext;

bool imguiVec3(const char* label, Vec3& vec, const char* format, ImGuiInputTextFlags flags);
bool imguiVec2(const char* label, Vec2& vec, const char* format, ImGuiInputTextFlags flags);
bool imguiRotation(const char* label, float* v, float step, float step_fast, const char* format, ImGuiInputTextFlags flags);


template<class T>
bool imguiComponentDeleteButton(World* world, T& component, Entity entity){
	std::string buttonName = "X##" + std::string{type_name<T>()};
	if(ImGui::Button(buttonName.c_str())){
		world->removeComponent<T>(entity);
		return true;
	}else{
		ImGui::SameLine();
	}
	return false;
}

void imguiSeparator(float widthRatio);



template <class T>
bool imguiComponent(World* world, ServiceContext* serviceContext, T& component, Entity entity);
 
template <>
bool imguiComponent<Transform>(World* world, ServiceContext* serviceContext, Transform& c, Entity entity);

template <>
bool imguiComponent<Hierarchy>(World* world, ServiceContext* serviceContext, Hierarchy& c, Entity entity);

template <>
bool imguiComponent<RigidBody2D>(World* world, ServiceContext* serviceContext, RigidBody2D& c, Entity entity);

template <>
bool imguiComponent<ParticleEmisor>(World* world, ServiceContext* serviceContext, ParticleEmisor& c, Entity entity);

template <>
bool imguiComponent<Sprite>(World* world, ServiceContext* serviceContext, Sprite& c, Entity entity);

template <>
bool imguiComponent<SpriteAnimation>(World* world, ServiceContext* serviceContext, SpriteAnimation& c, Entity entity);

template <>
bool imguiComponent<AudioSource>(World* world, ServiceContext* serviceContext, AudioSource& c, Entity entity);

template <>
bool imguiComponent<LightSource>(World* world, ServiceContext* serviceContext, LightSource& c, Entity entity);
template <>
bool imguiComponent<Trail>(World* world, ServiceContext* serviceContext, Trail& c, Entity entity);

bool imguiCollisionFiltering(std::string label, uint16_t* field, World* world, Entity entity);

bool imguiFieldTemplate(std::string label, char* field, World* world, ServiceContext* serviceContext, Entity entity);
bool imguiFieldTemplate(std::string label, bool* field, World* world, ServiceContext* serviceContext, Entity entity);
bool imguiFieldTemplate(std::string label, int* field, World* world, ServiceContext* serviceContext, Entity entity);
bool imguiFieldTemplate(std::string label, uint16_t* field, World* world, ServiceContext* serviceContext, Entity entity);
bool imguiFieldTemplate(std::string label, unsigned int* field, World* world, ServiceContext* serviceContext, Entity entity);
bool imguiFieldTemplate(std::string label, uint64_t* field, World* world, ServiceContext* serviceContext, Entity entity);
bool imguiFieldTemplate(std::string label, float* field, World* world, ServiceContext* serviceContext, Entity entity);
bool imguiFieldTemplate(std::string label, std::string* field, World* world, ServiceContext* serviceContext, Entity entity);
bool imguiFieldTemplate(std::string label, Vec2* field, World* world, ServiceContext* serviceContext, Entity entity);
bool imguiFieldTemplate(std::string label, Vec3* field, World* world, ServiceContext* serviceContext, Entity entity);
bool imguiFieldTemplate(std::string label, Color3* field, World* world, ServiceContext* serviceContext, Entity entity);
bool imguiFieldTemplate(std::string label, Color4* field, World* world, ServiceContext* serviceContext, Entity entity);

template <class T1, class T2>
bool imguiFieldTemplate(std::string label, std::tuple<T1, T2>* field, World* world, ServiceContext* serviceContext, Entity entity){
	auto& v0 = std::get<0>(*field);
	auto& v1 = std::get<1>(*field);
	std::string label0 = label + "##0";
	std::string label1 = label + "##1";
	bool changed = false;
	if constexpr (visit_struct::traits::is_visitable<  typename std::remove_reference<decltype(v0)>::type  >::value){
		changed |= imguiFieldTemplateVisitor(world, serviceContext, v0, entity); 
	}else if constexpr (std::is_enum<  typename std::remove_reference<decltype(v0)>::type  >::value){
		int* a = reinterpret_cast<int*>(&v0);
		changed |= imguiFieldTemplate(label0, a, world, serviceContext, entity);
	}else{
		changed |= imguiFieldTemplate(label0, &v0, world, serviceContext, entity);
	}

	if constexpr (visit_struct::traits::is_visitable<  typename std::remove_reference<decltype(v1)>::type  >::value){
		changed |= imguiFieldTemplateVisitor(world, serviceContext, v1, entity); 
	}else if constexpr (std::is_enum<  typename std::remove_reference<decltype(v1)>::type  >::value){
		int* a = reinterpret_cast<int*>(&v1);
		changed |= imguiFieldTemplate(label1, a, world, serviceContext, entity);
	}else{
		changed |= imguiFieldTemplate(label1, &v1, world, serviceContext, entity);
	}

	return changed;
}

template <class T>
bool imguiFieldTemplate(std::string label, std::vector<T>* field, World* world, ServiceContext* serviceContext, Entity entity){
	int i=0;
	bool changed = false;
	ImGui::TableNextColumn();
	ImGui::Text(label.c_str());
	ImGui::TableNextColumn();
	if(ImGui::Button(("New...##" + label).c_str())){
		field->push_back({});
		changed = true;
	}

	auto it = std::begin(*field);
	int up = -1;
	while(it != std::end(*field)){
		ImGui::TableNextColumn();
		ImGui::TableNextColumn();
		if(ImGui::Button(("X##" + label + std::to_string(i)).c_str())){
			it = field->erase(it);
			changed = true;
			i += 1;
			continue;
		}
		ImGui::SameLine();
		if(i != 0){
			if(ImGui::Button(("Up##" + label + std::to_string(i)).c_str())){
				up = i;
			}
		}

		ImGui::SameLine();
		if(ImGui::TreeNodeEx((std::to_string(i)+"##"+label).c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth)){
			if constexpr (visit_struct::traits::is_visitable<T>::value){
				changed |= imguiFieldTemplateVisitor(world, serviceContext, *it, entity); 
			}else{
				changed |= imguiFieldTemplate(label + std::to_string(i), &(*it), world, serviceContext, entity); 
			}
			ImGui::Separator();
			ImGui::TreePop();
		}
		++it;
		i++;
	}
	if(up != -1){
		auto aux = field->at(up-1);
		field->at(up-1) = field->at(up);
		field->at(up) = aux;
	}
	return changed;
}
//
//TODO: this
template <class T>
bool imguiFieldTemplate(std::string label, std::unordered_set<T>* field, World* world, ServiceContext* serviceContext, Entity entity){
	std::cout << "Imgui not implemented for std::unordered_set " << std::endl;
	return false;
}

template <typename... Args>
bool imguiFieldTemplate(std::string label, std::variant<Args...>* field, World* world, ServiceContext* serviceContext, Entity entity){
	bool changed = false;
	variant_by_index<std::variant<Args...>> type_indexer;
	std::size_t variantCount = std::variant_size_v<std::variant<Args...>>;
	int item_current_idx = field->index();

	std::string str = std::visit([](auto arg) {return type_name<decltype(arg)>().data();}, type_indexer.make_default(item_current_idx));
	str.pop_back();
	ImGui::TableNextColumn();
	ImGui::Text(label.c_str());
	ImGui::TableNextColumn();
	if(ImGui::BeginCombo(("##" + label).c_str(), str.c_str())){
		for (int n = 0; n < variantCount; n++){
			const bool is_selected = (item_current_idx == n);
			str = std::visit([](auto arg) {return type_name<decltype(arg)>().data();}, type_indexer.make_default(n));
			str.pop_back();
			if (ImGui::Selectable(str.c_str()))
				item_current_idx = n;
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		if(field->index() != item_current_idx){
			*field = type_indexer.make_default(item_current_idx);
			changed = true;
		}
		ImGui::EndCombo();
	}
	std::visit([&](auto& arg){
//		changed |= imguiFieldTemplateVisitor(world, serviceContext, arg, entity); 
		if constexpr (visit_struct::traits::is_visitable<  typename std::remove_reference<decltype(arg)>::type  >::value){
			changed |= imguiFieldTemplateVisitor(world, serviceContext, arg, entity); 
		}else{
			changed |= imguiFieldTemplate(label+std::to_string(0), &arg, world, serviceContext, entity); 
		}
	}, *field);

	imguiSeparator(0.5);
	return changed;
}

template <class T>
bool imguiFieldTemplateVisitor(World* world, ServiceContext* serviceContext, T& component, Entity entity) {
	bool changed = false;
	visit_struct::for_each(component,
	[&](const char * name, auto& value) {
		if constexpr (visit_struct::traits::is_visitable<  typename std::remove_reference<decltype(value)>::type  >::value){
			changed |= imguiFieldTemplateVisitor(world, serviceContext, value, entity); 
		}else if constexpr (std::is_enum<  typename std::remove_reference<decltype(value)>::type  >::value){
			int* a = reinterpret_cast<int*>(&value);
			changed |= imguiFieldTemplate(name, a, world, serviceContext, entity);
		}else{
			changed |= imguiFieldTemplate(name, &value, world, serviceContext, entity);
		}
	});
	return changed;
}

template <>
bool imguiFieldTemplateVisitor<ColorCorrection>(World* world, ServiceContext* serviceContext, ColorCorrection& component, Entity entity);

template <>
bool imguiFieldTemplateVisitor<ColorMixer>(World* world, ServiceContext* serviceContext, ColorMixer& component, Entity entity);


template <class T>
bool imguiComponentTemplate(World* world, ServiceContext* serviceContext, T& component, Entity entity) {
	bool changed{false};
	changed |= imguiComponentDeleteButton(world, component, entity);
	std::string typeName = type_name<T>().data();
	typeName.pop_back();
	int flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
	if(ImGui::TreeNodeEx(typeName.c_str(), flags)){
		if(ImGui::BeginTable("split", 2)){
			ImGui::TableSetupColumn("Field", ImGuiTableColumnFlags_WidthFixed , ImGui::GetContentRegionAvail().x/4);
			changed |= imguiFieldTemplateVisitor(world, serviceContext, component, entity);
			ImGui::EndTable();
		}
		ImGui::TreePop();
		ImGui::Separator();
	}
	return changed;
}

template <class T>
bool imguiComponent(World* world, ServiceContext* serviceContext, T& component, Entity entity) {
	return imguiComponentTemplate(world, serviceContext, component, entity);
}

template <class T>
bool imguiDataStruct(World* world, ServiceContext* serviceContext, T& component, Entity entity) {
	std::string typeName = type_name<T>().data();
	typeName.pop_back();
	bool changed{false};
	if(ImGui::BeginTable("split", 2)){
		ImGui::TableSetupColumn("Field", ImGuiTableColumnFlags_WidthFixed , ImGui::GetContentRegionAvail().x/4);
		changed |= imguiFieldTemplateVisitor(world, serviceContext, component, entity);
		ImGui::EndTable();
	}
	ImGui::Separator();
	return changed;
}


bool fileExplorerDialogDir(std::filesystem::path directory, std::string& selectedPath, int& node_clicked, int& i, int& selection_mask, int node_flags);
std::string showDirectoryHierarchy(std::string root, std::string currentDirectory);
bool fileExplorerDialogLoad(const char* label, std::string& selectedFileOutput, bool& cancel, ServiceContext* service, bool editName=false, std::string requiredExtension="");
