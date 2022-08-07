#include <string_view>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "commonStructures/EngineStatistics.hpp"
#include "EngineStatisticsImgui.hpp"

void printLineImgui(std::string_view name, int value){
	ImGui::TableNextColumn();
	ImGui::Text("%s", name.data());
	ImGui::TableNextColumn();
	ImGui::Text("%d", value);
}

void printLineImgui(std::string_view name, float value){
	ImGui::TableNextColumn();
	ImGui::Text("%s", name.data());
	ImGui::TableNextColumn();
	ImGui::Text("%.2f", value);
}

void printLineImgui_ms(std::string_view name, float value){
	ImGui::TableNextColumn();
	ImGui::Text("%s", name.data());
	ImGui::TableNextColumn();
	ImGui::Text("%.2f ms", value);
}



void printGraphicsStatisticsImgui(GraphicsStatistics* graphicsStatistics){
	ImGui::TableNextColumn();
	ImGui::Text("=== Graphics =============");
	ImGui::TableNextColumn();

	printLineImgui("drawCalls", graphicsStatistics->drawCalls);
	printLineImgui("quadsRendered", graphicsStatistics->quadsRendered);
	printLineImgui("linesRendered", graphicsStatistics->linesRendered);
	printLineImgui("circlesRendered", graphicsStatistics->circlesRendered);
	printLineImgui("texturesBinded", graphicsStatistics->texturesBinded);
	printLineImgui("particlesCount", graphicsStatistics->particlesCount);
	printLineImgui("fps", graphicsStatistics->fps);
}

void printCoreEngineStatisticsImgui(CoreEngineStatistics* coreEngineStatistics){
	ImGui::TableNextColumn();
	ImGui::Text("=== Core Engine =========");
	ImGui::TableNextColumn();

	printLineImgui("entitiesCount", coreEngineStatistics->entitiesCount);
	for(auto&[str, value]: coreEngineStatistics->systemTime){
		printLineImgui_ms(str, value);
	}
}


void printEngineStatisticsImgui(EngineStatistics* engineStatistics){
	if(ImGui::BeginTable("split", 2)){
		//ImGui::TableSetupColumn("StatisticsCollumn", ImGuiTableColumnFlags_WidthFixed , ImGui::GetContentRegionAvail().x/4);
		printGraphicsStatisticsImgui(&engineStatistics->graphicsStatistics);
		printCoreEngineStatisticsImgui(&engineStatistics->coreEngineStatistics);
		ImGui::EndTable();
	}
}
