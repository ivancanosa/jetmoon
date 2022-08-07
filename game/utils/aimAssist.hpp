#pragma once

#include "core/World.hpp"
#include "core/definitions.hpp"
#include "core/interpolation.hpp"

template <class T>
Vec2 aimAssist(World* world, const T& targets, Entity origin, Vec2 input, float dxAssist= 0.1, float dyAssist=0.01){
	float inputAngle = atan(input.y/input.x);
	std::vector<Vec2> points;
	auto originPos = fromMatrix(world->reduceTransformMatrix(origin)).position;
	Vec2 originVec{originPos.x, originPos.y};
	for(Entity target: targets){
		auto pos = fromMatrix(world->reduceTransformMatrix(origin)).position;
		Vec2 targetVec{pos.x, pos.y};
		targetVec -= originVec;
		float angle = atan(targetVec.y / targetVec.x);
		Vec2 v0{angle-dxAssist/2., angle-dyAssist/2.};
		Vec2 v1{angle+dxAssist/2., angle+dyAssist/2.};
		if(points.size() == 0){
			points.push_back(v0);
			points.push_back(v1);
		}else{
			bool removeLeft{false}, removeRight{false};
			float anteriorPos = 0.;
			for(int i=0; i<points.size(); i+=2){
				removeRight = v1.x >= points[i].x && v1.x <= points[i+1].x;
				removeLeft = v0.x >= points[i].x && v0.x <= points[i+1].x;
				if(removeRight && removeLeft){
					break;
				}else if(removeRight){
					points[i] = v0;
					break;
				}else if(removeLeft){
					points[i+1] = v1;
					break;
				}
				if(v0.x >= anteriorPos && v0.x <= points[i].x){
					points.insert(points.begin() + i, v1);
					points.insert(points.begin() + i, v0);
					break;
				}
				anteriorPos = points[i+1].x;
			}
		}
	}
	if(points.size() == 0){
		points.push_back({0., 0.});
		points.push_back({2*M_PI, 2*M_PI});
	}else{
		if(points[0].x > 0){
			points.insert(points.begin(), {0., 0.});
		}
		if(points[points.size()-1].x < 2*M_PI){
			points.push_back({2*M_PI, 2*M_PI});
		}
	}
	auto interpolator = monotoneCubicInterpolation(points);
	float outputAngle = interpolator(inputAngle);
	return {cos(outputAngle), sin(outputAngle)};
}
