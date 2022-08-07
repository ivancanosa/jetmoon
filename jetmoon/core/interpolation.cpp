#include "core/interpolation.hpp"

#include <vector>
#include <unordered_set>

std::function<float(float)> monotoneCubicInterpolation(const std::vector<Vec2>& points){
	if(points.size() < 3) return [](float t){return 0.;};
	std::vector<float> secantSlopes, slopes;
	std::unordered_set<int> ignorePos;
	std::vector<float> alpha, beta;
	secantSlopes.reserve(points.size()-1);
	alpha.resize(points.size()-1, 0.);
	beta.resize(points.size()-1, 0.);
	slopes.resize(points.size(), 0.);
	for(int i=0; i<points.size()-1; i++){
		secantSlopes.push_back((points[i+1].y - points[i].y)/(points[i+1].x - points[i].x));
	}
	for(int i=1; i<points.size()-1; i++){
		slopes[i] = (secantSlopes[i-1] + secantSlopes[i]/2.);
		if(secantSlopes[i] == 0.){
			slopes[i] = 0.;
			slopes[i+1] = 0.;
			ignorePos.insert(i);
			i += 1;
		}
	}
	slopes[0] = slopes[1];
	slopes[slopes.size()-1] = slopes[slopes.size()-2];

	for(int i=0; i<points.size()-1; i++){
		alpha[i] = slopes[i] / secantSlopes[i];
		beta[i] =  slopes[i+1] / secantSlopes[i];
	}
	for(int i=0; i<points.size()-1; i++){
		if(ignorePos.find(i) != ignorePos.end()){
			continue;
		}
		if(alpha[i]*alpha[i] + beta[i]*beta[i] > 9){
			float r = 3/sqrt(alpha[i]*alpha[i] + beta[i]*beta[i]);
			slopes[i] = r * alpha[i] * secantSlopes[i];
			slopes[i+1] = r * beta[i] * secantSlopes[i];
		}
	}

	return [points = points, slopes = slopes](float value){
		float h, t;
		int k;
		for(int i=0; i< points.size()-1; i++){
			if(points[i].x <= value && points[i+1].x >= value){
				k = i;
				h = points[i+1].x - points[i].x;
				t = (value - points[i].x) / h;
				break;
			}
		}
		return points[k].y*(2*t*t*t - 3*t*t + 1) + h * slopes[k] * (t*t*t - 2*t*t + t) + points[k+1].y * (-2*t*t*t + 3*t*t) + h * slopes[k+1] * (t*t*t - t*t);
	};
}

float randomNumberBetween(float left, float right){
	int resolution = 1000.;
	float n = float(rand()%resolution)/float(resolution);
	return n * (right - left) + left;
}

float randomNumberCentered(float center, float span){
	return randomNumberBetween(center - span/2, center + span/2);
}

float randomVec(const VecVar<float>& floatVar){
	return randomNumberCentered(floatVar.origin, floatVar.span);
}

Vec2 randomVec(const Vec2VarRotation& vec2){
	float beta = randomNumberCentered(0., vec2.var.y);
	Vec2 vec {
		(vec2.origin.x*cos(beta) - vec2.origin.y*sin(beta)),
		(vec2.origin.x*sin(beta) + vec2.origin.y*cos(beta))
	};
	Vec2 norm =  vec.getNormalized() * randomNumberCentered(0., vec2.var.x);
	return vec + norm;
}

Vec4 randomVec(const VecVar<Vec4>& vec4){
	return {
		randomNumberCentered(vec4.origin.x, vec4.span.x),
		randomNumberCentered(vec4.origin.y, vec4.span.y),
		randomNumberCentered(vec4.origin.z, vec4.span.z),
		randomNumberCentered(vec4.origin.w, vec4.span.w)
	};
}

float linearInterpolation(float v1, float v2, float t){
	return v1*(1.-t) + v2*t;
}

float smoothStart2(float v1, float v2, float t){
	return linearInterpolation(v1, v2, t*t);
}

float smoothStart3(float v1, float v2, float t){
	return linearInterpolation(v1, v2, t*t*t);
}

float smoothStart4(float v1, float v2, float t){
	return linearInterpolation(v1, v2, t*t*t*t);
}

float smoothStop2(float v1, float v2, float t){
	return linearInterpolation(v1, v2, 1 - (1-t)*(1-t));
}

float smoothStop3(float v1, float v2, float t){
	return linearInterpolation(v1, v2, 1 - (1-t)*(1-t)*(1-t));
}

float smoothStop4(float v1, float v2, float t){
	return linearInterpolation(v1, v2, 1 - (1-t)*(1-t)*(1-t)*(1-t));
}


float interpolation(float v1, float v2, float t, Interpolation interpolation){
	switch(interpolation){
		case INTERPOLATION_NULL:
			return v1;
		case INTERPOLATION_LINEAR:
			return linearInterpolation(v1, v2, t);

		case INTERPOLATION_SMOOTHSTART2:
			return smoothStart2(v1, v2, t);
		case INTERPOLATION_SMOOTHSTART3:
			return smoothStart3(v1, v2, t);
		case INTERPOLATION_SMOOTHSTART4:
			return smoothStart4(v1, v2, t);

		case INTERPOLATION_SMOOTHSTOP2:
			return smoothStop2(v1, v2, t);
		case INTERPOLATION_SMOOTHSTOP3:
			return smoothStop3(v1, v2, t);
		case INTERPOLATION_SMOOTHSTOP4:
			return smoothStop4(v1, v2, t);
	};
	return v1;
}

float interpolate(VecInterpolation<float> vecInter, float t){
	return interpolation(vecInter.origin, vecInter.destiny, t, vecInter.interpolation);
}

Vec2 interpolate(VecInterpolation<Vec2> vecInter, float t){
	return {
		interpolation(vecInter.origin.x, vecInter.destiny.x, t, vecInter.interpolation),
		interpolation(vecInter.origin.y, vecInter.destiny.y, t, vecInter.interpolation)
	};
}

Vec3 interpolate(VecInterpolation<Vec3> vecInter, float t){
	return {
		interpolation(vecInter.origin.x, vecInter.destiny.x, t, vecInter.interpolation),
		interpolation(vecInter.origin.y, vecInter.destiny.y, t, vecInter.interpolation),
		interpolation(vecInter.origin.z, vecInter.destiny.z, t, vecInter.interpolation)
	};
}


Vec4 interpolate(VecInterpolation<Vec4> vecInter, float t){
	return {
		interpolation(vecInter.origin.x, vecInter.destiny.x, t, vecInter.interpolation),
		interpolation(vecInter.origin.y, vecInter.destiny.y, t, vecInter.interpolation),
		interpolation(vecInter.origin.z, vecInter.destiny.z, t, vecInter.interpolation),
		interpolation(vecInter.origin.w, vecInter.destiny.w, t, vecInter.interpolation)
	};
}

Color3 interpolate(VecInterpolation<Color3> vecInter, float t){
	return {
		interpolation(vecInter.origin.x, vecInter.destiny.x, t, vecInter.interpolation),
		interpolation(vecInter.origin.y, vecInter.destiny.y, t, vecInter.interpolation),
		interpolation(vecInter.origin.z, vecInter.destiny.z, t, vecInter.interpolation)
	};
}


Color4 interpolate(VecInterpolation<Color4> vecInter, float t){
	return {
		interpolation(vecInter.origin.x, vecInter.destiny.x, t, vecInter.interpolation),
		interpolation(vecInter.origin.y, vecInter.destiny.y, t, vecInter.interpolation),
		interpolation(vecInter.origin.z, vecInter.destiny.z, t, vecInter.interpolation),
		interpolation(vecInter.origin.w, vecInter.destiny.w, t, vecInter.interpolation)
	};
}
