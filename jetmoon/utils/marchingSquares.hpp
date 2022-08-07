#pragma once
#include <tuple>                    // for tuple
#include <vector>                   // for vector
#include "../core/definitions.hpp"  // for Vec2

using PolygonVector = std::vector<std::tuple<bool, std::vector<Vec2>>>;

//true/1 is wall, false/0 is floor
//The bool indicate if the polygon is a hole
PolygonVector marchingSquares(std::vector<bool> matrix, int matrixWidth, int matrixHeight, Vec2 tileSize);
