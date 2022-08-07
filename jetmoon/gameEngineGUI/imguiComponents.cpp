#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <GL/glew.h>
#include <string>
#include <sstream>
#include <vector>
#include <variant>
#include <filesystem>

#include "core/definitions.hpp"
#include "core/interpolation.hpp"
#include "core/ServiceContext.hpp"
#include "gameEngineGUI/imguiComponents.hpp"
#include "gameEngineGUI/Gizmos.hpp"

#include "components/Transform.hpp"
#include "components/Sprite.hpp"
#include "components/ParticleEmisor.hpp"
#include "components/RigidBody2D.hpp"
#include "components/SpriteAnimation.hpp"

#include "services/AnimationDefinitionService.hpp"
#include "services/ImageLoader.hpp"

#include "utils/variant_by_index.hpp"
#include "utils/type_name.hpp"
#include "utils/visit_struct.hpp"

static const size_t strAuxiliarSize = 256;
static char strAuxiliar[strAuxiliarSize];

#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

//TODO: Transform camelCaseNase to a natural name
//Separate words, make the fist letter of each word upper case
std::string imguiCamelCaseToNatural(std::string camelCaseName){
	return {};
}

bool imguiCollisionFiltering(std::string label, uint16_t* field, World* world, Entity entity){
	bool changed{false};
	bool isSet{false};
	imguiSeparator(0.5);
	ImGui::Text("%s", label.c_str());
	if(ImGui::BeginTable("split", 9)){
		for(int i=0; i<18; i++){
			ImGui::TableNextColumn();
			int value{0};
			if(i == 4 || i == 13) continue;
			if(i > 13) value = i-2;
			else if(i > 4) value = i-1;
			else value = i;
			std::string newLabel = "##" + label + std::to_string(value);
			isSet = (*field & (1 << value));
			changed |= ImGui::Checkbox(newLabel.c_str(), &isSet);
			*field ^= (-(unsigned long)isSet ^ *field) & (1UL << value);
		}
		ImGui::EndTable();
	}
	return changed;
}

bool imguiFieldTemplate(std::string label, uint16_t* field, World* world, ServiceContext* serviceContext, Entity entity){
	bool changed{false};
	bool isSet{false};
	imguiSeparator(0.5);
	ImGui::TableNextColumn();
	ImGui::Text("%s", label.c_str());
	ImGui::TableNextColumn();
	if(ImGui::BeginTable("split", 9)){
		for(int i=0; i<18; i++){
			ImGui::TableNextColumn();
			int value{0};
			if(i == 4 || i == 13) continue;
			if(i > 13) value = i-2;
			else if(i > 4) value = i-1;
			else value = i;
			std::string newLabel = "##" + label + std::to_string(value);
			isSet = (*field & (1 << value));
			changed |= ImGui::Checkbox(newLabel.c_str(), &isSet);
			*field ^= (-(unsigned long)isSet ^ *field) & (1UL << value);
		}
		ImGui::EndTable();
	}
	return changed;
}

void imguiSeparator(float widthRatio){
	static int l = 0;
	l += 1;
	std::string s = "##Separator" + std::to_string(l);
	float c = ImGui::GetContentRegionAvail().x;
	float dx = c*(1-widthRatio)/2;
	float nw = widthRatio*c;
	ImGui::SetCursorPosX(dx);
	ImGui::BeginChild(s.c_str(), {nw, 10});
	ImGui::Separator();
	ImGui::EndChild();
}

bool imguiFieldTemplate(std::string label, char* field, World* world, ServiceContext* serviceContext, Entity entity){ return false; }

bool imguiFieldTemplate(std::string label, bool* field, World* world, ServiceContext* serviceContext, Entity entity){
	bool changed = false;
	ImGui::TableNextColumn();
	ImGui::Text("%s", label.c_str());
	ImGui::TableNextColumn();
	ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
	std::string floatLabel = "##" + label;
	changed |=  ImGui::Checkbox(floatLabel.c_str(), field);
	ImGui::PopItemWidth();
	return changed;
}

bool imguiFieldTemplate(std::string label, unsigned int* field, World* world, ServiceContext* serviceContext, Entity entity){
	bool changed = false;
	int a = *field;
	ImGui::TableNextColumn();
	ImGui::Text("%s", label.c_str());
	ImGui::TableNextColumn();
	ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
	std::string floatLabel = "##" + label;
	changed |= ImGui::InputInt(floatLabel.c_str(), &a);
	if(a < 0) a = 0;
	*field = a;
	ImGui::PopItemWidth();
	return changed;
}


bool InputUint64(const char* label, uint64_t* v, int step, int step_fast, ImGuiInputTextFlags flags) {
    const char* format = (flags & ImGuiInputTextFlags_CharsHexadecimal) ? "%08X" : "%d";
    return ImGui::InputScalar(label, ImGuiDataType_U64, (void*)v, (void*)(step > 0 ? &step : NULL), (void*)(step_fast > 0 ? &step_fast : NULL), format, flags);
}

bool imguiFieldTemplate(std::string label, uint64_t* field, World* world, ServiceContext* serviceContext, Entity entity){
	bool changed = false;
	uint64_t a = *field;
	ImGui::TableNextColumn();
	ImGui::Text("%s", label.c_str());
	ImGui::TableNextColumn();
	ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
	std::string floatLabel = "##" + label;
	changed |= InputUint64(floatLabel.c_str(), &a, 1, 1, 0);
	*field = a;
	ImGui::PopItemWidth();
	return changed;
}

bool imguiFieldTemplatePayload(std::string label, std::string* field, std::string payloadLabel){
	bool changed = false;
	ImGui::TableNextColumn();
	ImGui::Text("%s", label.c_str());
	ImGui::TableNextColumn();
	ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
	std::string stringInputLabel = "##" + label;
	strcpy(strAuxiliar, field->c_str());
	changed |= ImGui::InputText(stringInputLabel.c_str(), strAuxiliar, strAuxiliarSize);
	*field = strAuxiliar;

	if (ImGui::BeginDragDropTarget()) {
		if(const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(payloadLabel.c_str())){
			std::filesystem::path path = (char*)payload->Data;
			*field = path.stem().string();
		}
		ImGui::EndDragDropTarget();
	}

	ImGui::PopItemWidth();
	return changed;
}

bool imguiFieldTemplate(std::string label, int* field, World* world, ServiceContext* serviceContext, Entity entity){
	bool changed = false;
	ImGui::TableNextColumn();
	ImGui::Text("%s", label.c_str());
	ImGui::TableNextColumn();
	ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
	std::string floatLabel = "##" + label;
	changed |= ImGui::InputInt(floatLabel.c_str(), field);
	ImGui::PopItemWidth();
	return changed;
}

bool imguiFieldTemplate(std::string label, float* field, World* world, ServiceContext* serviceContext, Entity entity){
	bool changed = false;
	ImGui::TableNextColumn();
	ImGui::Text("%s", label.c_str());
	ImGui::TableNextColumn();
	ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
	std::string floatLabel = "##" + label;
	changed |= ImGui::InputFloat(floatLabel.c_str(), field);
	ImGui::PopItemWidth();
	return changed;
}

bool imguiFieldTemplateAngles(std::string label, float* field, World* world, ServiceContext* serviceContext, Entity entity){
	bool changed = false;
	ImGui::TableNextColumn();
	ImGui::Text("%s", label.c_str());
	ImGui::TableNextColumn();
	ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
	std::string floatLabel = "##" + label;
	changed |= ImGui::SliderAngle(floatLabel.c_str(), field, 0., 360.);
	ImGui::PopItemWidth();
	return changed;
}


bool imguiFieldTemplate(std::string label, std::string* field, World* world, ServiceContext* serviceContext, Entity entity){
	bool changed = false;
	ImGui::TableNextColumn();
	ImGui::Text("%s", label.c_str());
	ImGui::TableNextColumn();
	ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
	std::string stringInputLabel = "##" + label;
	strcpy(strAuxiliar, field->c_str());
	changed |= ImGui::InputText(stringInputLabel.c_str(), strAuxiliar, strAuxiliarSize);
	*field = strAuxiliar;
	ImGui::PopItemWidth();
	return changed;
}


bool imguiFieldTemplate(std::string label, Vec2* field, World* world, ServiceContext* serviceContext, Entity entity){
	bool changed = false;
	ImGui::TableNextColumn();
	ImGui::Text("%s", label.c_str());
	ImGui::TableNextColumn();
	ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x/2);
	std::string floatLabelX = "##" + label + "x";
	std::string floatLabelY = "##" + label + "y";
	changed |= ImGui::InputFloat(floatLabelX.c_str(), &field->x);
	ImGui::SameLine();
	changed |= ImGui::InputFloat(floatLabelY.c_str(), &field->y);
	ImGui::PopItemWidth();
	return changed;
}

bool imguiFieldTemplate(std::string label, Vec3* field, World* world, ServiceContext* serviceContext, Entity entity){
	bool changed = false;
	ImGui::TableNextColumn();
	ImGui::Text("%s", label.c_str());
	ImGui::TableNextColumn();
	std::string floatLabelX = "##" + label + "x";
	std::string floatLabelY = "##" + label + "y";
	std::string floatLabelZ = "##" + label + "z";
	ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x/3);
	changed |= ImGui::InputFloat(floatLabelX.c_str(), &field->x);
	ImGui::SameLine();
	changed |= ImGui::InputFloat(floatLabelY.c_str(), &field->y);
	ImGui::SameLine();
	changed |= ImGui::InputFloat(floatLabelZ.c_str(), &field->z);
	ImGui::PopItemWidth();
	return changed;
}

bool imguiFieldTemplate(std::string label, Color3* field, World* world, ServiceContext* serviceContext, Entity entity){
	bool changed = false;
	ImGui::TableNextColumn();
	ImGui::Text("Color");
	ImGui::TableNextColumn();
	changed |= ImGui::ColorEdit3(("##Color"+label).c_str(), &field->x, 0);
	return changed;
}

bool imguiFieldTemplate(std::string label, Color4* field, World* world, ServiceContext* serviceContext, Entity entity){
	bool changed = false;
	ImGui::TableNextColumn();
	ImGui::Text("Color");
	ImGui::TableNextColumn();
	changed |= ImGui::ColorEdit4(("##Color"+label).c_str(), &field->x, 0);
	return changed;
}

//=================================================
//========== Imgui Template Visitor ===============
//=================================================


template <>
bool imguiFieldTemplateVisitor<ColorCorrection>(World* world, ServiceContext* serviceContext, ColorCorrection& component, Entity entity){
	bool changed = false;
	ImGui::TableNextColumn();
	ImGui::Text("Contrast");
	ImGui::TableNextColumn();
	changed |= ImGui::InputFloat("##Contrast", &component.contrast, 0.1);

	ImGui::TableNextColumn();
	ImGui::Text("Brightness");
	ImGui::TableNextColumn();
	changed |= ImGui::InputFloat("##Brightness", &component.brightness, 0.1);

	ImGui::TableNextColumn();
	ImGui::Text("Saturation");
	ImGui::TableNextColumn();
	changed |= ImGui::InputFloat("##Saturation", &component.saturation, 0.1);

	ImGui::TableNextColumn();
	ImGui::Text("Gamma");
	ImGui::TableNextColumn();
	changed |= ImGui::InputFloat("##Gamma", &component.gamma, 0.1);

	ImGui::TableNextColumn();
	ImGui::Text("ColorFilter");
	ImGui::TableNextColumn();
	changed |= ImGui::ColorEdit3("##ColorFilter", &component.colorFiltering.x, 0);

	return changed;
}

template <>
bool imguiFieldTemplateVisitor<ColorMixer>(World* world, ServiceContext* serviceContext, ColorMixer& component, Entity entity){
	bool changed = false;
	ImGui::TableNextColumn();
	ImGui::Text("Intensity");
	ImGui::TableNextColumn();
	ImGui::InputFloat("##Intensity", &component.intensity, 0.1);

	ImGui::TableNextColumn();
	ImGui::Text("Color");
	ImGui::TableNextColumn();
	changed |= ImGui::ColorEdit4("##Color", &component.color.x, 0);
	return changed;
}


template <class T>
bool imguiComboBoxEnum(const char* label, T* value, const char* const enumStr[], size_t enumCount){
	int item_current_idx = (int)*value;
	bool changed{false};
	if(ImGui::BeginCombo(label, enumStr[item_current_idx])){
		for (int n = 0; n < enumCount; n++){
			const bool is_selected = (item_current_idx == n);
			if (ImGui::Selectable(enumStr[n], is_selected))
				item_current_idx = n;
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		if(*value != (T)item_current_idx){
			changed = true;
		}
		*value = (T)item_current_idx;
		ImGui::EndCombo();
	}
	return changed;
}


template <class T>
void imguiComboBoxVariant(const char* label, T* shape, const char* const variantStr[]){
	variant_by_index<T> type_indexer;
	std::size_t variantCount = std::variant_size_v<T>;
	int item_current_idx = shape->index();
	if(ImGui::BeginCombo(label, variantStr[item_current_idx])){
		for (int n = 0; n < variantCount; n++){
			const bool is_selected = (item_current_idx == n);
			if (ImGui::Selectable(variantStr[n], is_selected))
				item_current_idx = n;
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		if(shape->index() != item_current_idx){
			*shape = type_indexer.make_default(item_current_idx);
		}
		ImGui::EndCombo();
	}
}

bool imguiVec3(const char* label, Vec3& vec, const char* format, ImGuiInputTextFlags flags){
	float vecArray[3]{vec[0], vec[1], vec[2]};
	bool changed = ImGui::InputFloat3(label, vecArray, format, flags);
	vec.x = vecArray[0];
	vec.y = vecArray[1];
	vec.z = vecArray[2];
	return changed;
}

bool imguiVec2(const char* label, Vec2& vec, const char* format, ImGuiInputTextFlags flags){
	float vecArray[2]{vec[0], vec[1]};
	bool changed = ImGui::InputFloat2(label, vecArray, format, flags);
	vec.x = vecArray[0];
	vec.y = vecArray[1];
	return changed;
}

bool imguiRotation(const char* label, float* v, float step, float step_fast, const char* format, ImGuiInputTextFlags flags){
	float eulerAngles = *v * (360./(2*M_PI));
	bool changed = ImGui::InputFloat(label, &eulerAngles, step, step_fast, format, flags);
	*v = eulerAngles * ((2*M_PI)/360.);
	return changed;
}

bool imguiFloatVar(const char* label1, const char* label2, floatVar* floatVar, float step, float step_fast, const char* format, ImGuiInputTextFlags flags){
	bool changed = false;
	ImGui::PushItemWidth(100);
	changed |= ImGui::InputFloat(label1, &floatVar->origin, step, step_fast, format, flags);		
	ImGui::SameLine();
	ImGui::Text("+-");
	ImGui::SameLine();
	ImGui::PushItemWidth(100);
	changed |= ImGui::InputFloat(label2, &floatVar->span, step, step_fast, format, flags);		
	return changed;
}

bool imguiFloatVarAngles(const char* label1, const char* label2, floatVar* floatVar, float step, float step_fast, const char* format, ImGuiInputTextFlags flags){
	bool changed{false};
	ImGui::PushItemWidth(100);
	changed |= imguiRotation(label1, &floatVar->origin, step, step_fast, format, flags);		
	ImGui::SameLine();
	ImGui::Text("+-");
	ImGui::SameLine();
	ImGui::PushItemWidth(100);
	changed |= imguiRotation(label2, &floatVar->span, step, step_fast, format, flags);		
	return changed;
}


bool imguiInterpolationFloatAngles(const char* title, const char* inter, const char* l1, const char* l2, const char* l3, const char* l4, VecInterpolation<floatVar>* floatVar, float step, float step_fast, const char* format, ImGuiInputTextFlags flags){
	bool changed{false};
	ImGui::Text("%s", title);
	ImGui::Text("Interpolation:");
	ImGui::SameLine();
	ImGui::PushItemWidth(200);
	changed |= imguiComboBoxEnum(inter, &floatVar->interpolation, interpolationStr, IM_ARRAYSIZE(interpolationStr));
	changed |= imguiFloatVarAngles(l1, l2, &(floatVar->origin), step, step_fast, format, flags);
	changed |= imguiFloatVarAngles(l3, l4, &(floatVar->destiny), step, step_fast, format, flags);
	return changed;
}

bool imguiInterpolationFloat(const char* title, const char* inter, const char* l1, const char* l2, const char* l3, const char* l4, VecInterpolation<floatVar>* floatVar, float step, float step_fast, const char* format, ImGuiInputTextFlags flags){
	bool changed{false};
	ImGui::Text("%s", title);
	ImGui::Text("Interpolation:");
	ImGui::SameLine();
	ImGui::PushItemWidth(200);
	changed |= imguiComboBoxEnum(inter, &floatVar->interpolation, interpolationStr, IM_ARRAYSIZE(interpolationStr));
	changed |= imguiFloatVar(l1, l2, &(floatVar->origin), step, step_fast, format, flags);
	changed |= imguiFloatVar(l3, l4, &(floatVar->destiny), step, step_fast, format, flags);
	return changed;
}

bool imguiInterpolationFloat(const char* title, const char* inter, const char* l1, const char* l2, const char* l3, const char* l4, VecInterpolation<float>* floatVar, float step, float step_fast, const char* format, ImGuiInputTextFlags flags){
	bool changed{false};
	ImGui::Text("%s", title);
	ImGui::Text("Interpolation:");
	ImGui::SameLine();
	ImGui::PushItemWidth(200);
	changed |= imguiComboBoxEnum(inter, &floatVar->interpolation, interpolationStr, IM_ARRAYSIZE(interpolationStr));
	changed |= ImGui::InputFloat(l1, &floatVar->origin, step, step_fast, format, flags);		
	changed |= ImGui::InputFloat(l2, &floatVar->destiny, step, step_fast, format, flags);		
	return changed;
}

bool imguiInterpolationVec2Rotation(const char* title, const char* inter, const char* l1, const char* l2, const char* l3, const char* l4, VecInterpolation<Vec2VarRotation>* vecVar, ImGuiInputTextFlags flags){
	float value[2];
	bool changed{false};
	ImGui::Text("%s", title);
	ImGui::Text("Interpolation:");
	ImGui::SameLine();
	ImGui::PushItemWidth(200);
	changed |= imguiComboBoxEnum(inter, &vecVar->interpolation, interpolationStr, IM_ARRAYSIZE(interpolationStr));

	for(int i=0; i<2; i++)
		value[i] = vecVar->origin.origin[i];
	changed |= ImGui::InputFloat2(l1, value, 0);
	for(int i=0; i<2; i++)
		vecVar->origin.origin[i] = value[i];

	value[0] = vecVar->origin.var[0];
	value[1] = vecVar->origin.var[1] * 360./(2*M_PI);
	changed |= ImGui::InputFloat2(l2, value, 0);
	vecVar->origin.var[0] = value[0];
	vecVar->origin.var[1] = value[1] * (2*M_PI)/360.;

	for(int i=0; i<2; i++)
		value[i] = vecVar->destiny.origin[i];
	changed |= ImGui::InputFloat2(l3, value, 0);
	for(int i=0; i<2; i++)
		vecVar->destiny.origin[i] = value[i];

	value[0] = vecVar->destiny.var[0];
	value[1] = vecVar->destiny.var[1] * 360./(2*M_PI);
	changed |= ImGui::InputFloat2(l4, value, 0);
	vecVar->destiny.var[0] = value[0];
	vecVar->destiny.var[1] = value[1] * (2*M_PI)/360.;

	return changed;
}

bool imguiInterpolationColor(const char* title, const char* inter, const char* l1, const char* l2, const char* l3, const char* l4, VecInterpolation<Vec4Var>* vecInterp, ImGuiInputTextFlags flags){
	float color[4];
	bool changed{false};
	ImGui::Text("%s", title);
	ImGui::Text("Interpolation:");
	ImGui::SameLine();
	ImGui::PushItemWidth(200);
	changed |= imguiComboBoxEnum(inter, &vecInterp->interpolation, interpolationStr, IM_ARRAYSIZE(interpolationStr));

	for(int i=0; i<4; i++)
		color[i] = vecInterp->origin.origin[i];
	changed |= ImGui::ColorEdit4(l1, color, 0);
	for(int i=0; i<4; i++)
		vecInterp->origin.origin[i] = color[i];

	for(int i=0; i<4; i++)
		color[i] = vecInterp->origin.span[i];
	changed |= ImGui::ColorEdit4(l2, color, 0);
	for(int i=0; i<4; i++)
		vecInterp->origin.span[i] = color[i];

	for(int i=0; i<4; i++)
		color[i] = vecInterp->destiny.origin[i];
	changed |= ImGui::ColorEdit4(l3, color, 0);
	for(int i=0; i<4; i++)
		vecInterp->destiny.origin[i] = color[i];

	for(int i=0; i<4; i++)
		color[i] = vecInterp->destiny.span[i];
	changed |= ImGui::ColorEdit4(l4, color, 0);
	for(int i=0; i<4; i++)
		vecInterp->destiny.span[i] = color[i];
	return changed;
}


bool imguiInterpolationColor(const char* title, const char* inter, const char* l1, const char* l2, const char* l3, const char* l4, VecInterpolation<Color4>* vecInterp, ImGuiInputTextFlags flags){
	float color[4];
	bool changed{false};
	ImGui::Text("%s", title);
	ImGui::Text("Interpolation:");
	ImGui::SameLine();
	ImGui::PushItemWidth(200);
	changed |= imguiComboBoxEnum(inter, &vecInterp->interpolation, interpolationStr, IM_ARRAYSIZE(interpolationStr));

	for(int i=0; i<4; i++)
		color[i] = vecInterp->origin[i];
	changed |= ImGui::ColorEdit4(l1, color, 0);
	for(int i=0; i<4; i++)
		vecInterp->origin[i] = color[i];

	for(int i=0; i<4; i++)
		color[i] = vecInterp->destiny[i];
	changed |= ImGui::ColorEdit4(l3, color, 0);
	for(int i=0; i<4; i++)
		vecInterp->destiny[i] = color[i];

	return changed;
}

void imguiShape(PointShape& shape){ }
void imguiShape(RectangularShape& shape){
	ImGui::Text("Width, height");
	ImGui::PushItemWidth(100);
	ImGui::SameLine();
	ImGui::InputFloat("##recShape0", &shape.width, 0., 0., "%.1f", 0);
	ImGui::PushItemWidth(100);
	ImGui::SameLine();
	ImGui::InputFloat("##recShape1", &shape.height, 0., 0., "%.1f", 0);
}
void imguiShape(CircularShape& shape){
	ImGui::Text("Radio");
	ImGui::PushItemWidth(100);
	ImGui::SameLine();
	ImGui::InputFloat("##circShape0", &shape.radio, 0., 0., "%.1f", 0);
}
void imguiShape(RingShape& shape){
	ImGui::Text("Radio(inner, outer)");
	ImGui::PushItemWidth(100);
	ImGui::SameLine();
	ImGui::InputFloat("##ringShape0", &shape.innerRadio, 0., 0., "%.1f", 0);
	ImGui::PushItemWidth(100);
	ImGui::SameLine();
	ImGui::InputFloat("##ringShape1", &shape.outerRadio, 0., 0., "%.1f", 0);
}


//=======================================================================================
//========================== Component Templates ========================================
//=======================================================================================




template <>
bool imguiComponent<Hierarchy>(World* world, ServiceContext* serviceContext, Hierarchy& c, Entity entity){return false;}

void imguiInputResource(const char* label, std::string& source, std::string payloadLabel){
	strcpy(strAuxiliar, source.c_str());
	if(ImGui::InputText(label, strAuxiliar, strAuxiliarSize, ImGuiInputTextFlags_EnterReturnsTrue)){
		source.assign(strAuxiliar);
	}
	if (ImGui::BeginDragDropTarget()) {
		if(const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(payloadLabel.c_str())){
			source = (char*)payload->Data;
		}
		ImGui::EndDragDropTarget();
	}
}


template <>
bool imguiComponent<Sprite>(World* world, ServiceContext* serviceContext, Sprite& sprite, Entity entity) {
	bool changed{false};
	changed |= imguiComponentDeleteButton(world, sprite, entity);
	int flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
	if(ImGui::TreeNodeEx("Sprite", flags)){
		if(ImGui::BeginTable("split", 2)){
			ImGui::TableSetupColumn("Field", ImGuiTableColumnFlags_WidthFixed , ImGui::GetContentRegionAvail().x/4);
			changed |= imguiFieldTemplate("position", &sprite.position, world, serviceContext, entity);
			changed |= imguiFieldTemplate("size", &sprite.size, world, serviceContext, entity);
			changed |= imguiFieldTemplateAngles("rotation", &sprite.rotation, world, serviceContext, entity);
			changed |= imguiFieldTemplate("tiling", &sprite.tiling, world, serviceContext, entity);
			changed |= imguiFieldTemplate("color", &sprite.color, world, serviceContext, entity);
			changed |= imguiFieldTemplate("emisor", &sprite.emisor, world, serviceContext, entity);
			changed |= imguiFieldTemplatePayload("id", &sprite.id, "imagePayload");
			changed |= imguiFieldTemplate("flipX", &sprite.flipX, world, serviceContext, entity);
			changed |= imguiFieldTemplate("flipY", &sprite.flipY, world, serviceContext, entity);
			changed |= imguiFieldTemplate("visible", &sprite.visible, world, serviceContext, entity);
			ImGui::EndTable();
		}
		ImGui::TreePop();
		ImGui::Separator();
	}
	return changed;
}

template <>
bool imguiComponent<Transform>(World* world, ServiceContext* serviceContext, Transform& transform, Entity entity) {
	bool changed{false};
	changed |= imguiComponentDeleteButton(world, transform, entity);
	int flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
	if(ImGui::TreeNodeEx("Transform", flags)){
		if(ImGui::BeginTable("split", 2)){
			ImGui::TableSetupColumn("Field", ImGuiTableColumnFlags_WidthFixed , ImGui::GetContentRegionAvail().x/4);
			changed |= imguiFieldTemplate("position", &transform.position, world, serviceContext, entity);
			changed |= imguiFieldTemplate("scale", &transform.scale, world, serviceContext, entity);
			changed |= imguiFieldTemplateAngles("rotation", &transform.rotation, world, serviceContext, entity);

			ImGui::EndTable();
		}
		ImGui::TreePop();
		ImGui::Separator();
	}
	return changed;
}

template <>
bool imguiComponent<AudioSource>(World* world, ServiceContext* serviceContext, AudioSource& c, Entity entity){
	bool changed{false};
	changed |= imguiComponentDeleteButton(world, c, entity);
	int flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
	if(ImGui::TreeNodeEx("AudioSource", flags)){
		if(ImGui::BeginTable("split", 2)){
			ImGui::TableSetupColumn("Field", ImGuiTableColumnFlags_WidthFixed , ImGui::GetContentRegionAvail().x/4);
			changed |= imguiFieldTemplatePayload("audioId", &c.audioId, "audioPayload");
			changed |= imguiComboBoxEnum("audioGroup", &c.audioGroup, AudioGroupStr, IM_ARRAYSIZE(AudioGroupStr));
			changed |= imguiFieldTemplate("volume", &c.volume, world, serviceContext, entity);
			changed |= imguiFieldTemplate("pitch", &c.pitch, world, serviceContext, entity);
			changed |= imguiFieldTemplate("playInLoop", &c.playInLoop, world, serviceContext, entity);
			changed |= imguiFieldTemplate("isPositional", &c.isPositional, world, serviceContext, entity);

			changed |= imguiFieldTemplate("play", &c.play, world, serviceContext, entity);
			changed |= imguiFieldTemplate("reset", &c.reset, world, serviceContext, entity);
			ImGui::EndTable();
		}
		ImGui::TreePop();
		ImGui::Separator();
	}
	return changed;
}

template <>
bool imguiComponent<LightSource>(World* world, ServiceContext* serviceContext, LightSource& lightSource, Entity entity){
	bool changed{false};
	changed |= imguiComponentDeleteButton(world, lightSource, entity);
	int flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
	if(ImGui::TreeNodeEx("LightSource", flags)){
		if(ImGui::BeginTable("split", 2)){
			ImGui::TableSetupColumn("Field", ImGuiTableColumnFlags_WidthFixed , ImGui::GetContentRegionAvail().x/4);
			changed |= imguiFieldTemplate("radius", &lightSource.radius, world, serviceContext, entity);
			changed |= imguiFieldTemplate("intensity", &lightSource.intensity, world, serviceContext, entity);
			changed |= imguiFieldTemplate("color", &lightSource.color, world, serviceContext, entity);
			ImGui::EndTable();
		}
		ImGui::TreePop();
		ImGui::Separator();
	}
	return changed;
}

template <>
bool imguiComponent<Trail>(World* world, ServiceContext* serviceContext, Trail& trail, Entity entity){
	bool changed{false};
	changed |= imguiComponentDeleteButton(world, trail, entity);
	int flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
	if(ImGui::TreeNodeEx("Trail", flags)){
		if(ImGui::BeginTable("split", 2)){
			ImGui::TableSetupColumn("Field", ImGuiTableColumnFlags_WidthFixed , ImGui::GetContentRegionAvail().x/4);
			changed |= imguiFieldTemplate("time", &trail.time, world, serviceContext, entity);
			changed |= imguiInterpolationFloat("==== Width ", "##Interpolation1", "origin ##1", "", "destiny##1", "", &trail.width, 0., 0., "%.1f", 0);
			changed |= imguiInterpolationColor("==== Color ", "##Interpolation3", "origin ##3", "", "destiny##3", "", &trail.color, 0);
			changed |= imguiFieldTemplate("minVertexDistance", &trail.minVertexDistance, world, serviceContext, entity);
			changed |= imguiFieldTemplatePayload("id", &trail.texture, "imagePayload");
			changed |= imguiFieldTemplate("autodestruct", &trail.autodestruct, world, serviceContext, entity);
			changed |= imguiFieldTemplate("renderAsSprites", &trail.renderAsSprites, world, serviceContext, entity);

			ImGui::EndTable();
		}
		ImGui::TreePop();
		ImGui::Separator();
	}
	return changed;
}


template <>
bool imguiComponent<SpriteAnimation>(World* world, ServiceContext* serviceContext, SpriteAnimation& spriteAnimation, Entity entity) {
	bool changed{false};
	changed |= imguiComponentDeleteButton(world, spriteAnimation, entity);
	int flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
	static bool create{false};
	static bool save{false};
	bool exists{false};

	if(ImGui::TreeNodeEx("SpriteAnimation", flags)){
		if(ImGui::BeginTable("split", 2)){
			ImGui::TableSetupColumn("Field", ImGuiTableColumnFlags_WidthFixed , ImGui::GetContentRegionAvail().x/4);
			changed |= imguiFieldTemplate("position", &spriteAnimation.position, world, serviceContext, entity);
			changed |= imguiFieldTemplateAngles("rotation", &spriteAnimation.rotation, world, serviceContext, entity);
			changed |= imguiFieldTemplate("size", &spriteAnimation.size, world, serviceContext, entity);
			changed |= imguiFieldTemplate("flipX", &spriteAnimation.flipX, world, serviceContext, entity);
			changed |= imguiFieldTemplate("flipY", &spriteAnimation.flipY, world, serviceContext, entity);
			changed |= imguiFieldTemplate("visible", &spriteAnimation.visible, world, serviceContext, entity);
			changed |= imguiFieldTemplate("overrideDuration", &spriteAnimation.overrideDuration, world, serviceContext, entity);
			changed |= imguiFieldTemplate("duration", &spriteAnimation.duration, world, serviceContext, entity);
			changed |= imguiFieldTemplate("subAnimation", &spriteAnimation.subAnimation, world, serviceContext, entity);
			changed |= imguiFieldTemplatePayload("controller", &spriteAnimation.animationController, "animationControllerPayload");
			exists = serviceContext->animationDefinitionService->exists(spriteAnimation.animationController);
			if(!exists){
				if(create || ImGui::Button("Create Controller...")){
					create = true;
					std::string output;
					bool cancel{false};
					if(fileExplorerDialogLoad("Save Controller as...", output, cancel, serviceContext, true, ".animationController")){
						create = false;
						if(!cancel){
							std::filesystem::path path = output;
							std::string name = path.stem().string();
							spriteAnimation.animationController = name;
							serviceContext->animationDefinitionService->loadAnimation(name, {});
						}
					}

				}
			}
			ImGui::EndTable();
			if(exists && ImGui::TreeNodeEx("AnimationControllerEdit", ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding)){
				
				AnimationDefinition animationDefinition = serviceContext->animationDefinitionService->getAnimationDefinition(spriteAnimation.animationController);
				serviceContext->animationDefinitionService->removeAnimation(spriteAnimation.animationController);
				if(save || ImGui::Button("Save controller...")){
					save = true;
					std::string output;
					bool cancel{false};
					if(fileExplorerDialogLoad("Save Controller as...", output, cancel, serviceContext, true, ".animationController")){
						save = false;
						if(!cancel){
							nlohmann::json j(animationDefinition);
							std::string s{j.dump(2)};
							std::string pathStr{output};
							if(std::filesystem::exists(output)) std::remove(pathStr.c_str());
							std::ofstream out(output);
							out << s;
							out.close();
						}
					}

				}

				if(ImGui::BeginTable("split", 2)){
					ImGui::TableNextColumn();
					ImGui::TableNextColumn();
					imguiFieldTemplate("spriteSheet", &animationDefinition.id, world, serviceContext, entity);
					imguiFieldTemplate("margin", &animationDefinition.margin, world, serviceContext, entity);
					imguiFieldTemplate("padding", &animationDefinition.padding, world, serviceContext, entity);
					imguiFieldTemplate("frameWidth", &animationDefinition.frameWidth, world, serviceContext, entity);
					imguiFieldTemplate("frameHeight", &animationDefinition.frameHeight, world, serviceContext, entity);
					ImGui::Separator();
					ImGui::EndTable();
					int i=0;

					if(ImGui::Button("New Subanimation...")){
						animationDefinition.subAnimations.push_back({std::to_string(i), 0, 0, 1., false});
					}
					for(auto&[key, row, finalColumn, duration, isLoop]: animationDefinition.subAnimations){
						if(ImGui::Button(("X##" + std::to_string(i)).c_str())){
							animationDefinition.subAnimations.erase(animationDefinition.subAnimations.begin() + i);
							continue;
						}
						ImGui::SameLine();
						if(ImGui::TreeNodeEx(std::to_string(i).c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth)){
							if(ImGui::BeginTable("split", 2)){
								imguiFieldTemplate("id", &key, world, serviceContext, entity);
								imguiFieldTemplate("row", &row, world, serviceContext, entity);
								imguiFieldTemplate("finalColumn", &finalColumn, world, serviceContext, entity);
								imguiFieldTemplate("duration", &duration, world, serviceContext, entity);
								imguiFieldTemplate("isLoop", &isLoop, world, serviceContext, entity);
								ImGui::EndTable();
							}
							ImGui::Separator();
							ImGui::TreePop();
						}
						i += 1;
					}
				}
				ImGui::TreePop();
				serviceContext->animationDefinitionService->loadAnimation(spriteAnimation.animationController, animationDefinition);
			}	

		}
		ImGui::TreePop();
		ImGui::Separator();
	}

	return changed;
}

template <>
bool imguiComponent<RigidBody2D>(World* world, ServiceContext* serviceContext, RigidBody2D& rigidBody, Entity entity) {
	int flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
	bool changed{false};
	changed |= imguiComponentDeleteButton(world, rigidBody, entity);
	if(ImGui::TreeNodeEx("Rigid Body 2D", flags)){
		if(ImGui::BeginTable("split", 2)){
			ImGui::TableSetupColumn("Field", ImGuiTableColumnFlags_WidthFixed , ImGui::GetContentRegionAvail().x/4);
			changed |= imguiFieldTemplate("position", &rigidBody.position, world, serviceContext, entity);
			//changed |= imguiFieldTemplate("rotation", &rigidBody.rotation);
			changed |= imguiFieldTemplateAngles("rotation", &rigidBody.rotation, world, serviceContext, entity);



			changed |= imguiFieldTemplate("fixedRotation", &rigidBody.fixedRotation, world, serviceContext, entity);
			ImGui::EndTable();

			changed |= imguiComboBoxEnum("BodyType", &rigidBody.bodyType, bodyTypeStr, IM_ARRAYSIZE(bodyTypeStr));
			int i = 0;	
			if(ImGui::Button("New Fixture...")){
				rigidBody.fixtureVector.emplace_back(Fixture());
			}
			int flags =  ImGuiTreeNodeFlags_SpanFullWidth;

			for(auto& fixture: rigidBody.fixtureVector){

				if(i != 0){
					std::string buttonName = "X##fixture" + std::to_string(i);
					if(ImGui::Button(buttonName.c_str())){
						rigidBody.fixtureVector.erase(rigidBody.fixtureVector.begin() + i);
						changed = true;
						i += 1;
					}
					ImGui::SameLine();
				}
/*				if(i==0){
					auto tr = world->reduceComponent<Transform>(entity);
					float position[3] = {fixture.position.x+tr.position.x, fixture.position.y+tr.position.y, 0.};
					float scale[2] = {(float)1. * tr.scale.x, (float)1. * tr.scale.y};
					float rotation[1] = {(float)0. + tr.rotation};
					std::visit([&](auto&& arg) {
						using T = std::decay_t<decltype(arg)>;
						if constexpr (std::is_same_v<T, RectangularShape>){
							scale[0] = arg.width * tr.scale.x;
							scale[1] = arg.height * tr.scale.y;
							rotation[0] = arg.rotation + tr.rotation;
							Gizmos::renderGizmos2D(position, scale, rotation);
							fixture.position.x = position[0] - tr.position.x;
							fixture.position.y = position[1] - tr.position.y;
							arg.width = scale[0] / tr.scale.x;
							arg.height = scale[1] / tr.scale.y;
							arg.rotation = rotation[0] - tr.rotation;
						}else{ 
							Gizmos::renderGizmos2D(position, scale, rotation);
							fixture.position.x = position[0] - tr.position.x;
							fixture.position.y = position[1] - tr.position.y;
						}
					}, fixture.shape);
					changed = true;
				}*/
				std::string label = "Fixture " + std::to_string(i);
				if( ImGui::TreeNodeEx(label.c_str(), flags) ){
					std::string labelTable = "split" + label;
					if(ImGui::BeginTable(labelTable.c_str(), 2)){
						ImGui::TableSetupColumn("Field", ImGuiTableColumnFlags_WidthFixed , ImGui::GetContentRegionAvail().x/4);
						changed |= imguiFieldTemplate("position", &fixture.position, world, serviceContext, entity);			
						changed |= imguiFieldTemplateAngles("rotation", &fixture.rotation, world, serviceContext, entity);			

						imguiFieldTemplate("enable", &fixture.enable, world, serviceContext, entity);
						changed |= imguiFieldTemplate("tag", &fixture.tag, world, serviceContext, entity);			
						changed |= imguiFieldTemplate("isSensor", &fixture.isSensor, world, serviceContext, entity);			
						changed |= imguiFieldTemplate("density", &fixture.density, world, serviceContext, entity);			
						changed |= imguiFieldTemplate("friction", &fixture.friction, world, serviceContext, entity);			
						changed |= imguiFieldTemplate("restitution", &fixture.restitution, world, serviceContext, entity);			
						changed |= imguiFieldTemplate("shape", &fixture.shape, world, serviceContext, entity);			
						ImGui::EndTable();
						changed |= imguiCollisionFiltering("Collision Group", &fixture.collisionGroup, world, entity);			
						changed |= imguiCollisionFiltering("Collision Mask", &fixture.collisionMask, world, entity);			
					}
					ImGui::TreePop();
				}
				i += 1;
			}
		}
		ImGui::TreePop();
		ImGui::Separator();
		rigidBody.updateBody = changed;
	}
	return changed;
}

template <>
bool imguiComponent<ParticleEmisor>(World* world, ServiceContext* serviceContext, ParticleEmisor& particleEmisor, Entity entity) {
	bool changed{false};
	changed |= imguiComponentDeleteButton(world, particleEmisor, entity);
	int flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
	if(ImGui::TreeNodeEx("ParticleEmisor", flags)){
		imguiFieldTemplatePayload("texture", &particleEmisor.texture, "imagePayload");
		ImGui::Text("Generation Rate");
		ImGui::SameLine();
		changed |= ImGui::InputFloat("##generation", &particleEmisor.continuousGenerationRate, 0., 0., "%.1f", 0);

		ImGui::Text("Burst Count    ");
		ImGui::SameLine();
		changed |= ImGui::InputInt("##burstCount", &particleEmisor.burstCount, 0);

		ImGui::Text("Continuous     ");
		ImGui::SameLine();
		changed |= ImGui::Checkbox("##continuous", &particleEmisor.continuous);

		ImGui::Text("Lifetime");
		ImGui::SameLine();
		changed |= imguiFloatVar("##lifetime", "##lifetime+-", &particleEmisor.lifetime, 0., 0., "%.1f", 0);		

		ImGui::Text("Rotation");
		ImGui::SameLine();
		changed |= imguiFloatVarAngles("##rotation", "##rotation+-", &particleEmisor.rotation, 0., 0., "%.1f", 0);		

		ImGui::Text("==== Shape");
		ImGui::SameLine();
		ImGui::PushItemWidth(180);

		changed |= imguiFieldTemplate("##shape", &particleEmisor.spawnShape, world, serviceContext, entity);

//		imguiComboBoxVariant("##shape", &particleEmisor.spawnShape, spawnShapeStr);
//		std::visit([](auto&& arg){imguiShape(arg); }, particleEmisor.spawnShape);

		changed |= imguiInterpolationFloatAngles("==== Rotation Speed ","##Interpolation0", "origin ##0", "##origin+-0", "destiny##0", "##destiny+-0", &particleEmisor.rotationSpeed, 0., 0., "%.0f", 0);
		changed |= imguiInterpolationFloat("==== Size ", "##Interpolation1", "origin ##1", "##origin+-1", "destiny##1", "##destiny+-1", &particleEmisor.size, 0., 0., "%.1f", 0);
		changed |= imguiInterpolationVec2Rotation("==== Speed ", "##Interpolation2", "origin(x,y) ##2", "+-(modulus, angle)##origin+-2", "destiny(x,y)##2", "+-(modulus, angle)##destiny+-2", &particleEmisor.speed, 0);
		changed |= imguiInterpolationColor("==== Color ", "##Interpolation3", "origin ##3", "+-##origin+-3", "destiny##3", "+-##destiny+-3", &particleEmisor.color, 0);

		ImGui::TreePop();
		ImGui::Separator();
	}

	return changed;
}

bool fileExplorerDialogDir(std::filesystem::path directory, std::string& selectedPath, int& node_clicked, int& i, int& selection_mask, int node_flags){
	i += 1;
	const bool is_selected = (selection_mask & (1 << i)) != 0;
	auto relativePath = std::filesystem::relative(directory);
	std::string filenameString = relativePath.stem().string();
	std::string iconName = "folder";
	ImGui::PushID(filenameString.c_str());
	//ImGui::ImageButton(reinterpret_cast<ImTextureID>(imageID), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

	if(is_selected){
		node_flags |= ImGuiTreeNodeFlags_Selected;
		selectedPath = relativePath.string();
	}
	bool node_open = ImGui::TreeNodeEx(filenameString.c_str(), node_flags);

	if(ImGui::IsItemClicked()){
		node_clicked = i;
	}

	if(node_clicked != -1){
		if (ImGui::GetIO().KeyCtrl)
			selection_mask ^= (1 << node_clicked);          
		else 
			selection_mask = (1 << node_clicked);
	}
	
	ImGui::PopID();

	return node_open;
}

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

std::string showDirectoryHierarchy(std::string root="data", std::string currentDirectory="data"){
	int i{0};
	int node_clicked{-1};
	static int selection_mask{0};
	std::string selectedPath = root;

	const std::filesystem::path rootPath = root;
	showDirectoryHierarchyRec(rootPath, node_clicked, i, selection_mask, selectedPath);
	return selectedPath;
}

std::vector<std::string> split(const std::string str, char delim) {
	std::vector<std::string> result;
	std::istringstream ss{str};
	std::string token;
	while (std::getline(ss, token, delim)) {
		if (!token.empty()) {
			result.push_back(token);
		}
	}
	return result;
}

bool fileExplorerDialogLoad(const char* label, std::string& selectedFileOutput, bool& cancel, ServiceContext* service, bool editName, std::string requiredExtension){
	const std::filesystem::path dataPath = "data";
	static float padding = 16.0f;
	static float thumbnailSize = 64.0f;
	static std::filesystem::path selectedFile{""};
	float cellSize = thumbnailSize + padding;
	bool isExit{false};

	GLint viewport[4]{};
	glGetIntegerv(GL_VIEWPORT, viewport);
	ImVec2 size{(float)viewport[2]/2, (float)viewport[3]/2};
	ImGui::Begin(label);
	ImGui::SetWindowSize(size);
	ImGui::SetWindowPos({(float)viewport[2]/2 - size.x/2, (float)viewport[3]/2 - size.y/2});
	static std::filesystem::path currentDirectory = "data";
	if(ImGui::BeginTable("split", 2, 0, {size.x, size.y - 7 * ImGui::GetTextLineHeight()})){
		ImGui::TableSetupColumn("C0", ImGuiTableColumnFlags_WidthFixed , size.x/4);
		ImGui::TableNextColumn();

		std::string path = showDirectoryHierarchy("data", currentDirectory.string());
		currentDirectory = path;
		ImGui::TableNextColumn();

		auto vec = split(path, '/');
		for(int i=0; i<vec.size(); i++){
			ImGui::Text("%s", vec[i].c_str());
			ImGui::SameLine();
			ImGui::Text("/");
			if(i != vec.size() - 1)
				ImGui::SameLine();
		}
		float panelWidth = ImGui::GetContentRegionAvail().x;

		ImGui::Separator();
		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;
		if(ImGui::BeginTable("split", columnCount)){

			ImGui::TableNextColumn();

			for (auto& directoryEntry : std::filesystem::directory_iterator(currentDirectory)){
				const auto& path = directoryEntry.path();
				auto relativePath = std::filesystem::relative(path, dataPath);
				std::string filenameString = relativePath.stem().string();
				if(directoryEntry.is_directory()) continue;
				std::string iconName = "file";
				std::string filenameWithoutExtension = relativePath.stem().string();
				std::string extension = directoryEntry.path().extension().string();

				if(requiredExtension != ""){
					if(extension != requiredExtension){
						continue;
					}
				}
				if(service->imageLoader->isLoaded(filenameWithoutExtension)){
					iconName = filenameWithoutExtension;
				}
				ImGui::PushID(filenameString.c_str());
				GLuint imageID = service->imageLoader->load(iconName);
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
				ImGui::ImageButton(reinterpret_cast<ImTextureID>(imageID), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });
				if(ImGui::IsItemClicked()){
					if(!editName)
						selectedFile = currentDirectory.string() + "/" + relativePath.stem().string();
					else
						selectedFile = relativePath.stem().string();
				}
				ImGui::PopStyleColor();
				ImGui::TextWrapped("%s", filenameString.c_str());
				ImGui::TableNextColumn();
				ImGui::PopID();
			}
			ImGui::EndTable();
		}
		ImGui::EndTable();
	}

	ImGui::Separator();
	if(ImGui::BeginTable("split", 2)){
		ImGui::TableSetupColumn("C0", ImGuiTableColumnFlags_WidthFixed , size.x/4);
		ImGui::TableNextColumn();
		ImGui::TableNextColumn();

		if(selectedFile != "" || editName){
			ImGui::Text("File: ");
			ImGui::SameLine();
			if(!editName){
				ImGui::Text("%s", selectedFile.stem().string().c_str());
			}else{
				strcpy(strAuxiliar, selectedFile.string().c_str());
				ImGui::InputText("##InputTextFileExplorerDialog", strAuxiliar, strAuxiliarSize);
				selectedFile = strAuxiliar;
			}
			if(requiredExtension != ""){
				ImGui::SameLine();
				ImGui::Text("%s", requiredExtension.c_str());
			}
			if(ImGui::Button("Cancel")){
				cancel = true;
				isExit = true;
			}
			ImGui::SameLine();
			if(ImGui::Button("Accept")){
				if(!editName){
					selectedFileOutput = selectedFile.string() + requiredExtension;
				}else{
					selectedFileOutput = currentDirectory.string() + "/" + selectedFile.string() + requiredExtension;
				}
				selectedFile = "";
				isExit = true;
			}
		}else{
			if(ImGui::Button("Cancel")){
				cancel = true;
				isExit = true;
			}
		}
		ImGui::EndTable();
	}

	ImGui::End();
	return isExit;
}


bool imguiAcceptDialog(std::string* description, bool& result){
	GLint viewport[4]{};
	ImVec2 size{};
	size.x = 400;
	size.y = 200;
	glGetIntegerv(GL_VIEWPORT, viewport);
	ImGui::Begin("ImguiAcceptDialog");
	ImGui::SetWindowSize({size.x, size.y});
	ImGui::SetWindowPos({(float)viewport[2]/2 - size.x/2, (float)viewport[3]/2});

	ImGui::Text("%s", description->c_str());
	if(ImGui::Button("Accept")){
		result = true;
		return true;
	}
	ImGui::SameLine();
	if(ImGui::Button("Cancel")){
		result = false;
		return true;
	}
	return false;
}
