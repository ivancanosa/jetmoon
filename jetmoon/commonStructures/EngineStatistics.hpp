#pragma once
#include <string>
#include <string_view>
#include <tuple>
#include <vector>
#include "utils/ComponentInspector.hpp"

struct GraphicsStatistics{
	int drawCalls{0};
	int pointsRendered{0};
	int linesRendered{0};
	int trianglesRendered{0};
	int quadsRendered{0};
	int circlesRendered{0};
	int texturesBinded{0};
	int particlesCount{0};
	float fps{0.};

	void clear(){
		drawCalls = 0;
		pointsRendered = 0;
		linesRendered = 0;
		trianglesRendered = 0;
		quadsRendered = 0;
		circlesRendered = 0;
		texturesBinded = 0;
		particlesCount = 0;
		fps = 0;
	}
};

COMPONENT_INSPECTOR(GraphicsStatistics, drawCalls, quadsRendered, linesRendered, circlesRendered, texturesBinded, particlesCount, fps);

struct CoreEngineStatistics{
	int entitiesCount{0};
	std::vector<std::tuple<std::string, float>> systemTime{};

	void clear(){
		entitiesCount = 0;
		systemTime.clear();
	}
};

COMPONENT_INSPECTOR(CoreEngineStatistics, entitiesCount, systemTime);

struct EngineStatistics{
	GraphicsStatistics graphicsStatistics{};
	CoreEngineStatistics coreEngineStatistics{};

	void clear(){
		graphicsStatistics.clear();
		coreEngineStatistics.clear();
	}
};

COMPONENT_INSPECTOR(EngineStatistics, graphicsStatistics, coreEngineStatistics);
