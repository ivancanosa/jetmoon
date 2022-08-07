#pragma once

#include "commonStructures/Camera.hpp"
#include "commonStructures/LightningConfig.hpp"
#include "commonStructures/Navmesh.hpp"
#include <box2d/box2d.h>

struct b2WorldDS{
	b2World* world{nullptr};
	b2Vec2 gravity{0.f, -0.f};
	float worldScale{1.f};
	bool showColliders{false};
	bool createdWorld{false};

	b2WorldDS(){
	}

	b2WorldDS(const b2WorldDS& b2world){
		gravity = b2world.gravity;
		worldScale = b2world.worldScale;
		showColliders = b2world.showColliders;
		world = nullptr;
		createdWorld = false;
	}

	~b2WorldDS(){
		if(createdWorld){
			delete world;
		}
	}
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(b2Vec2, x, y);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(b2WorldDS, gravity, worldScale);

struct WorldContext{
	WorldContext() = default;
	WorldContext(const WorldContext& worldContext){
		b2World = b2WorldDS(worldContext.b2World);
		camera = worldContext.camera;
		lightningConfig = worldContext.lightningConfig;
		navmesh = worldContext.navmesh;
	}

	b2WorldDS b2World{};
	Camera camera{};
	LightningConfig lightningConfig{};
	Navmesh navmesh{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(WorldContext, b2World, camera, lightningConfig, navmesh);
