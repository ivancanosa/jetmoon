#include <vector>
#include <tuple>

#include <memory>
#include <cmath>                  // for sqrt, abs
#include <cstdlib>                // for abs, size_t

#include "utils/astar.hpp"
#include "core/definitions.hpp"
#include "Navmesh.hpp"

inline int crossProduct(const Vec2& a, const Vec2& b, const Vec2& c) {
    return ((b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x));
}
inline bool isLeft(const Vec2& a, const Vec2& b, const Vec2& c) {
    return (crossProduct(a, b, c) >= 0);
}
inline bool inTriangle(const Vec2& a, const Vec2& b, const Vec2& c, const Vec2& x) {
    return (isLeft(x,a,b) == isLeft(x,b,c) && isLeft(x,b,c) == isLeft(x,c,a));
}
int Navmesh::b_search(int left, int right, const Vec2& Q, int polygon) {
    int i = left;
    int j = right;
    while (i < j-1)
    {
        int mid = (i+j)/2;
		Vec2 a = verticesVector[polysVector[polygon][0]];
		Vec2 b = verticesVector[polysVector[polygon][mid]];
        if (isLeft(a, b, Q)) i = mid;
        else j = mid;
    }
    return i;
}

bool Navmesh::inPolygon(const Vec2& Q, int polygon) {
    int pos = b_search(1, polysVector[polygon].size()-1, Q, polygon);
	Vec2 a = verticesVector[polysVector[polygon][0]];
	Vec2 b = verticesVector[polysVector[polygon][pos]];
	Vec2 c = verticesVector[polysVector[polygon][pos+1]];
    return inTriangle(a, b, c, Q);
}


inline double triarea2(const Vec2& a, const Vec2& b, const Vec2& c) {
	const float ax = b.x - a.x;
	const float ay = b.y - a.y;
	const float bx = c.x - a.x;
	const float by = c.y - a.y;
	return bx*ay - ax*by;
}

inline double getVectorOrder(const Vec2& a, const Vec2& b, const Vec2& center){
	return (a.x - center.x) * (b.y - center.y) - (b.x - center.x) * (a.y - center.y);
}

inline float vdistsqr(const Vec2& a, const Vec2 b){
	const float dx = a.x - b.x ;
	const float dy = a.y - b.y;
	return dx*dx + dy*dy;
}

inline bool vequal(const Vec2& a, const Vec2& b) {
	static const float thr = (1.0f/16384.0f) * (1.0f/16384.0f);
	return vdistsqr(a, b) < thr;
}

std::vector<Vec2> stringPull(const std::vector<Vec2>& portals){
	// Find straight path.
	std::vector<Vec2> pts;
	// Init scan state
	Vec2 portalApex, portalLeft, portalRight;
	int apexIndex = 0, leftIndex = 0, rightIndex = 0;
	portalApex = portals[0];
	portalLeft = portals[0];
	portalRight = portals[1];

	// Add start point.
	pts.push_back(portalApex);
	int nportals = portals.size() / 2;

	for (int i = 1; i < nportals; ++i) {
		Vec2 left = portals[i*2+0];
		Vec2 right = portals[i*2+1];

		// Update right vertex.
		if (triarea2(portalApex, portalRight, right) <= 0.0f) {
			if (vequal(portalApex, portalRight) || triarea2(portalApex, portalLeft, right) > 0.0f) {
				// Tighten the funnel.
				portalRight = right;
				rightIndex = i;
			}
			else {
				// Right over left, insert left to path and restart scan from portal left point.
				pts.push_back(portalLeft);
				// Make current left the new apex.
				portalApex = portalLeft;
				apexIndex = leftIndex;
				// Reset portal
				portalLeft = portalApex;
				portalRight = portalApex;
				leftIndex = apexIndex;
				rightIndex = apexIndex;
				// Restart scan
				i = apexIndex;
				continue;
			}
		}

		// Update left vertex.
		if (triarea2(portalApex, portalLeft, left) >= 0.0f) {
			if (vequal(portalApex, portalLeft) || triarea2(portalApex, portalRight, left) < 0.0f) {
				// Tighten the funnel.
				portalLeft = left;
				leftIndex = i;
			}
			else {
				// Left over right, insert right to path and restart scan from portal right point.
				pts.push_back(portalRight);
				// Make current right the new apex.
				portalApex = portalRight;
				apexIndex = rightIndex;
				// Reset portal
				portalLeft = portalApex;
				portalRight = portalApex;
				leftIndex = apexIndex;
				rightIndex = apexIndex;
				// Restart scan
				i = apexIndex;
				continue;
			}
		}
	}
	// Append last point to path.
	pts.push_back(portals[(nportals-1)*2]);

	return pts;
}

void Navmesh::clear(){
	generate = false;
	verticesVector.clear();
	polysVector.clear();
	graph.clear();
	centersVector.clear();
}


inline float manhattanDistance(const Vec2& p0, const Vec2& p1){
	return std::abs(p1.x - p0.x) + std::abs(p1.y - p0.y);
}

inline float euclideanDistance(const Vec2& p0, const Vec2& p1){
	return  std::sqrt((p1.x - p0.x)*(p1.x - p0.x) + (p1.y - p0.y)*(p1.y - p0.y));
}

inline float heuristicDistance(const Vec2& p0, const Vec2& p1){
	return  std::sqrt((p1.x - p0.x)*(p1.x - p0.x) + (p1.y - p0.y)*(p1.y - p0.y));
}

struct PolyNode{
	int polyIndex;
	Vec2 pos;

	bool operator==(const PolyNode& n) const{
		return polyIndex == n.polyIndex;
	}
};

namespace std { //Hash function for the std::tuple<int,int> type
  template <> struct hash<PolyNode> {
    size_t operator()(const PolyNode & x) const {
		return x.polyIndex;
    }
  };
}

int Navmesh::getNearest(Vec2 point){
	int nearest0{0}, nearest1{0}, nearest2{0}, nearest3{0};
	float distance0{999999}, distance1{999999}, distance2{999999}, distance3{999999}, newDist;
	int size = centersVector.size();
	for(int i=0; i < size; i++){
		newDist = manhattanDistance(point, centersVector[i]);
		if(distance0 > newDist){
			distance3 = distance2;
			distance2 = distance1;
			distance1 = distance0;
			distance0 = newDist;
			nearest3 = nearest2;
			nearest2 = nearest1;
			nearest1 = nearest0;
			nearest0 = i;
		}else if(distance1 > newDist){
			distance3 = distance2;
			distance2 = distance1;
			distance1 = newDist;
			nearest3 = nearest2;
			nearest2 = nearest1;
			nearest1 = i;
		}else if(distance2 > newDist){
			distance3 = distance2;
			distance2 = newDist;
			nearest3 = nearest2;
			nearest2 = i;
		}else if(distance3 > newDist){
			distance3 = newDist;
			nearest3 = i;
		}
	}
	if(inPolygon(point, nearest0)){
		return nearest0;
	}else if(inPolygon(point, nearest1)){
		return nearest1;
	}else if(inPolygon(point, nearest2)){
		return nearest2;
	}
	return nearest3;
}



std::vector<Vec2> Navmesh::getPath(Vec2 start, Vec2 goal){
	if(polysVector.size() == 0) return {};

	int startGraph = getNearest(start);
	int goalGraph = getNearest(goal);
	PolyNode startN = {startGraph, start};
	PolyNode goalN = {goalGraph, centersVector[goalGraph]};

	auto nextNodes = [&](const auto& node){
		std::vector<PolyNode> nexts{};
		//nexts.clear();
		float dist0, dist1, dist2;
		for(auto [nextGraph, v0, v1]: graph[node.polyIndex]){
			Vec2 mean = (v0+v1)/2;
			dist0 = manhattanDistance(v0, node.pos);
			dist1 = manhattanDistance(v1, node.pos);
			dist2 = manhattanDistance(mean, node.pos);
			if(dist0 < dist1 && dist0 < dist2){
				nexts.push_back({nextGraph, v0});
			}else if(dist1 < dist0 && dist1 < dist2){
				nexts.push_back({nextGraph, v1});
			}else{
				nexts.push_back({nextGraph, mean});
			}
		}
		return std::move(nexts);
	};
	auto heuristicLambda = [&](const auto& node, const auto& goal){
		return heuristicDistance(node.pos, goal.pos);
	};
	auto edgeCost = [&](const auto& node0, const auto& node1){
		return euclideanDistance(node0.pos, node1.pos);
	};

//=======================================
	auto path = fastAStar(startN, goalN, nextNodes, heuristicLambda, edgeCost);

	Vec2 init = start;
	std::vector<Vec2> portals{};
	portals.push_back(init);
	portals.push_back(init);
	for(int i=0; i < path.size() - 1; i++){
		int graph0 = path[i].polyIndex;
		int nG = path[i+1].polyIndex;
		for(auto[graph1, left, right]: graph[graph0]){
			if(nG == graph1){
				Vec2 center = centersVector[graph0];
				auto a = getVectorOrder(left, right, center);
				if(a < 0.){
					portals.push_back(left);
					portals.push_back(right);
				}else{
					portals.push_back(right);
					portals.push_back(left);
				}
				break;
			}
		}
	}
	portals.push_back(goal);
	portals.push_back(goal);

	return stringPull(portals);
}


