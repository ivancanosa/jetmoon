
#include "jetmoon.h"

std::shared_ptr<World> createPlayableWorld(std::string scenePath=""){
	auto entitiesLoader = [=](World* world){
		if(scenePath != ""){
			SceneSerializer serializer;
			serializer.loadWorld(world, scenePath);
		}
	};

	static auto componentLoader = [](World* world){
		world->registerComponent<NullComponent>();
		variant_by_index<ComponentVariant> type_indexer;
		std::size_t variantCount = std::variant_size_v<ComponentVariant>;
		for (std::size_t i=0; i < variantCount; i++){
			std::visit([=](auto arg){world->registerComponent<decltype(arg)>();}, type_indexer.make_default(i));
		}
	};

	static auto systemLoader = [](World* world){
		world->addTimeline("core");
		world->addTimeline("update");
		world->addTimeline("render");

		world->registerSystem<PrefabSystem, PrefabComponent>("core");
		world->registerSystem<Physics, Transform, RigidBody2D>("core");


		world->registerSystem<CameraSystem, CameraComponent>("update");

		world->registerSystem<AudioSystem, AudioSource>("render");
		world->registerSystem<RenderClear, NullComponent>("render");
		world->registerSystem<ProceduralTextureSystem, ProceduralTexture>("render");
		world->registerSystem<RenderSprite, Sprite>("render");
		world->registerSystem<RenderAnimation, SpriteAnimation>("render");
		world->registerSystem<TrailSystem, Transform, Trail>("render");
		world->registerSystem<TilemapSystem, TilemapComponent>("render");
#ifndef __RELEASE__
		world->registerSystem<SaveEntityMapSystem, NullComponent>("render");
#endif
		world->registerSystem<NavmeshSystem, Transform, RigidBody2D>("render"); //TODO:this is temporal
		world->registerSystem<ParticleSystem, ParticleEmisor>("render");
		world->registerSystem<Light2DSystem, LightSource>("render");

		world->registerSystem<PostProcessingSystem, NullComponent>("render");
		world->registerSystem<VolumetricPostProcessingSystem, PostProcessing, RigidBody2D, CollisionSensor>("render");

#ifndef __RELEASE__
		world->registerSystem<DebugPhysics, NullComponent>("render");
		world->registerSystem<DebugGUI, CameraComponent>("render");
#endif
		world->registerSystem<RenderDisplay, NullComponent>("render");
	};

	auto world = std::make_shared<World>();
	world->setComponentLoader(componentLoader);
	world->setSystemLoader(systemLoader);
	world->setEntitiesLoader(entitiesLoader);
	return world;
}
