#include "marchingSquares.hpp"
#include <assert.h>                 // for assert
#include <array>                    // for array
#include <set>                      // for set, set<>::iterator
#include <tuple>                    // for tuple, get
#include <vector>                   // for vector, vector<>::reference
#include <tuple>
#include "../core/definitions.hpp"  // for Vec2

static char invertDirection(char dir){
	static std::array<char, 4> table = {2, 3, 0, 1};
	return table[dir];
}

static std::tuple<char, char> getDirections(char binaryIndex){
	static std::vector<std::tuple<char,char>> table = {
		{-1, -1},
		{3, 2},
		{2, 1},
		{3, 1},
		{1, 0},
		{},
		{2, 0},
		{3, 0},
		{0, 3},
		{0, 2},
		{},
		{0, 1},
		{1, 3},
		{1, 2},
		{2, 3},
		{-1, -1}
	};
	return table[binaryIndex];
}

static std::tuple<unsigned int, unsigned int> applyDirection(unsigned int x, unsigned int y, char dir){
	switch(dir){
		case 0:
			return {x, y-1};
		case 1:
			return {x+1, y};
		case 2:
			return {x, y+1};
		case 3:
			return {x-1, y};
		default:
			return {x+1, y};
	}
}

void removeVerticesInSeries(PolygonVector& solution){
	for(auto& [_, vec]: solution){
		for(int i=0; i<vec.size()-2; i++){
			Vec2 p0 = vec[i];
			Vec2 p1 = vec[i+1];
			Vec2 p2 = vec[i+2];
			if(p0.x == p1.x && p1.x == p2.x){
				vec.erase(vec.begin() + i + 1);
				i -= 1;
			}else if(p0.y == p1.y && p1.y == p2.y){
				vec.erase(vec.begin() + i + 1);
				i -= 1;
			}
		}
	}
}

//true/1 is wall, false/0 is floor
std::vector<std::tuple<bool, std::vector<Vec2>>> marchingSquares(std::vector<bool> matrix, int matrixWidth, int matrixHeight, Vec2 tileSize){
	auto index = [=](unsigned int x, unsigned int y)->unsigned int{return x + y*matrixWidth;};
	auto indexN = [=](unsigned int x, unsigned int y)->unsigned int{return x + y*(matrixWidth-1);};
	PolygonVector solution{};
	std::vector<unsigned char> binaryIndex{};
	std::set<std::tuple<unsigned int, unsigned int>> borders{}; //have indexN of the matrix that have borders
	std::set<std::tuple<unsigned int, unsigned int>> doubleBorder{}; //have indexN of the matrix that have borders
	std::array<char, 4> value5Map = {3, 2, 1, 0};
	std::array<char, 4> value10Map = {1, 0, 3, 2};
	//binaryIndex.reserve((matrixWidth-1) * (matrixHeight-1));
	binaryIndex.reserve((matrixWidth) * (matrixHeight));
	for(unsigned int y=0; y < matrixHeight-1; y++){
		for(unsigned int x=0; x < matrixWidth-1; x++){
			unsigned char value = 0;
			value |= matrix[index(x, y)] << 3;
			value |= matrix[index(x+1, y)] << 2;
			value |= matrix[index(x+1, y+1)] << 1;
			value |= matrix[index(x, y+1)] << 0;
			if(value != 0 && value != 15){
				borders.insert({x, y});
				if(value == 5 || value == 10)
					doubleBorder.insert({x,y});
			}
			binaryIndex.push_back(value);
		}
	}

	int polygonId = 0;
	while(!borders.empty()){
		solution.push_back({true, {}});
		auto it = borders.begin();
		auto[x, y] = *it;
		auto value = binaryIndex[indexN(x, y)];
		while(value ==  5 || value == 10){ // this fails if every border is 5 or 10 -> chess pattern
			it = std::next(it);
			assert(it != borders.end() && "End of the borders set");
			auto[x2, y2] = *it;
			x = x2;
			y = y2;
			value = binaryIndex[indexN(x, y)];
		}
		borders.erase({x, y});

		if(matrix[index(x, y)]){ //Detect if the polygon is hole or not
			std::get<0>(solution[polygonId]) = false;
		}else{
			std::get<0>(solution[polygonId]) = true;
		}

		auto[d1, d2] = getDirections(binaryIndex[indexN(x, y)]);
		auto anteriorDir = invertDirection(d1);
		Vec2 p1{(float)(x+1)*tileSize.x,(float)(y+1)*tileSize.y};
		unsigned int origin = indexN(x,y);
		int ox = x;
		int oy = y;
		auto[x2, y2] = applyDirection(x, y, d1);
		x = x2;
		y = y2;
		Vec2 p2{(float)(x+1)*tileSize.x,(float)(y+1)*tileSize.y};
		std::get<1>(solution[polygonId]).push_back(p1);
		std::get<1>(solution[polygonId]).push_back(p2);
		while(borders.find({x,y}) != borders.end()){
			if(doubleBorder.find({x,y}) == doubleBorder.end()){
				borders.erase({x, y});
			}else{
				doubleBorder.erase({x,y});
			}
			auto value = binaryIndex[indexN(x,y)];
			char actualDir;
			switch(value){
				case 5:
					actualDir = value5Map[anteriorDir];
					break;
				case 10:
					actualDir = value10Map[anteriorDir];
					break;
				default:
					auto[d1, d2] = getDirections(binaryIndex[indexN(x, y)]);
					actualDir = d1;
					if(anteriorDir == d1)
						actualDir = d2;
			}
			if(anteriorDir == invertDirection(actualDir)){
				std::get<1>(solution[polygonId]).pop_back();
			}
			anteriorDir = invertDirection(actualDir);
			auto[x2, y2] = applyDirection(x, y, actualDir);
			x = x2;
			y = y2;
			Vec2 p2{(float)(x+1)*tileSize.x,(float)(y+1)*tileSize.y};
			std::get<1>(solution[polygonId]).push_back(p2);
		}
		if(origin != indexN(x,y)){
			std::vector<Vec2> auxVec{};
			auto[d1, d2] = getDirections(binaryIndex[indexN(ox, oy)]);
			x = ox;
			y = oy;
			auto actualDir = d2;
			auto anteriorDir = invertDirection(d2);
			auto[x2, y2] = applyDirection(x, y, actualDir);
			x = x2;
			y = y2;
			Vec2 p2{(float)(x+1)*tileSize.x,(float)(y+1)*tileSize.y};
			auxVec.push_back(p2);
			while(borders.find({x,y}) != borders.end()){
				if(doubleBorder.find({x,y}) == doubleBorder.end()){
					borders.erase({x, y});
				}else{
					doubleBorder.erase({x,y});
				}
				auto value = binaryIndex[indexN(x,y)];
				char actualDir;
				switch(value){
					case 5:
						actualDir = value5Map[anteriorDir];
						break;
					case 10:
						actualDir = value10Map[anteriorDir];
						break;
					default:
						auto[d1, d2] = getDirections(binaryIndex[indexN(x, y)]);
						actualDir = d1;
						if(anteriorDir == d1)
							actualDir = d2;
				}
				if(anteriorDir == invertDirection(actualDir)){
					auxVec.pop_back();
				}

				anteriorDir = invertDirection(actualDir);
				auto[x2, y2] = applyDirection(x, y, actualDir);
				x = x2;
				y = y2;
				Vec2 p2{(float)(x+1)*tileSize.x,(float)(y+1)*tileSize.y};
				auxVec.push_back(p2);
			}
			std::get<1>(solution[polygonId]).insert(std::get<1>(solution[polygonId]).begin(), auxVec.rbegin(), auxVec.rend());
		}else{
			std::get<1>(solution[polygonId]).pop_back();
		}

		polygonId++;
	}
	removeVerticesInSeries(solution);
	return solution;
}
