#pragma once

#include <stdlib.h>
#include <box2d/box2d.h>


class b2WorldService{
public:
	b2WorldService(){
		world = new b2World(gravity);
	}

	~b2WorldService(){
		delete world;
	}

	b2World* world{};
	b2Vec2 gravity{0.f, -6.f};
	float worldScale = 1.f;
};
