#pragma once

#include <string_view>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "commonStructures/EngineStatistics.hpp"

void printLineImgui(std::string_view name, int value);
void printLineImgui(std::string_view name, float value);

void printGraphicsStatisticsImgui(GraphicsStatistics* graphicsStatistics);
void printCoreEngineStatisticsImgui(CoreEngineStatistics* coreEngineStatistics);

void printEngineStatisticsImgui(EngineStatistics* engineStatistics);
