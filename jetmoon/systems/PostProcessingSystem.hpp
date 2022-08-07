#pragma once
#include <string_view>                      // for string_view
#include "core/ISystem.hpp"                 // for ISystem
#include "opengl/PostProcessingHelper.hpp"  // for PostProcessingHelper
class World;
struct ServiceContext;
struct WorldContext;


class PostProcessingSystem: public ISystem{
	public:
		void init(World* world, WorldContext* worldContext, ServiceContext* serviceContext) override; 

		void update(World* world, WorldContext* worldContext, ServiceContext* serviceContext) override;

		std::string_view getName() override;

		ISystem* clone() override;

	private:
		PostProcessingHelper postProcessingHelper;
};
