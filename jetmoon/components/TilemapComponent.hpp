#pragma once

#include "utils/ComponentInspector.hpp"

#include "core/definitions.hpp"


struct TilemapComponent{
	std::string tilemap{};	
	bool regenerate{true};
	uint16_t collisionGroup{0x0000000000000001};
	uint16_t collisionMask {0x0000000000000001};

	// Read only field
	bool generated{false};
	std::vector<bool> collisionMatrix{};
	int matrixSizeX{0};
	int matrixSizeY{0};

	std::vector<bool> isFixtureHole{};
};

COMPONENT_INSPECTOR(TilemapComponent, tilemap, regenerate, collisionGroup, collisionMask);
