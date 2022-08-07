#pragma once

#include <vector>
#include <tuple>
#include <stdint.h>              // for uint16_t
#include "utils/json.hpp"        // for basic_json, NLOHMANN_DEFINE_TYPE_NON...

#include "core/definitions.hpp"

class Navmesh{
public:
	void clear();

	std::vector<Vec2> getPath(Vec2 start, Vec2 goal);
	void reducePathByVisibility(std::vector<Vec2>& path);

	bool generate{false}, render{false};
	uint16_t collisionGroup{0x0000000000000001};
	float agentRadius{0.01};

	std::vector<Vec2> verticesVector{};
	std::vector<std::vector<int>> polysVector;

	std::vector<std::vector<std::tuple<int, Vec2, Vec2>>> graph{};
	std::vector<Vec2> centersVector{};

private:
	int b_search(int left, int right, const Vec2& Q, int polygon);
	bool inPolygon(const Vec2& Q, int polygon);

	int getNearest(Vec2 point);
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Navmesh, generate, render, collisionGroup, agentRadius, verticesVector, polysVector, graph, centersVector);
