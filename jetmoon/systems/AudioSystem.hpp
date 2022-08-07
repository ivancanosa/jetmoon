#pragma once
#include <array>                 // for array
#include <string_view>           // for string_view
#include "core/ISystem.hpp"      // for ISystem
#include "core/definitions.hpp"  // for Entity, MAX_ENTITIES
#include "utils/miniaudio.h"     // for ma_sound
class AudioService;
class World;
struct ServiceContext;
struct WorldContext;


class AudioSystem: public ISystem{
	public:
		void removedEntity(Entity entity, World* world, WorldContext* worldContext, ServiceContext* serviceContext) override;

		void init(World* world, WorldContext* worldContext, ServiceContext* serviceContext) override;

		void update(World* world, WorldContext* worldContext, ServiceContext* serviceContext) override;
		ISystem* clone() override;
		std::string_view getName() override;
		~AudioSystem();
	private:
		std::array<ma_sound, MAX_ENTITIES> soundsArray;
		AudioService* audioService{nullptr};
};
