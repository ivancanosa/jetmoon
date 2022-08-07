#pragma once
#include <string_view>       // for string_view
#include "core/ISystem.hpp"  // for ISystem
#include "commonStructures/ConfigDS.hpp"    
class World;
struct ServiceContext;
struct WorldContext;


class RenderClear: public ISystem{

public:
	void init(World* world, WorldContext* worldContext, ServiceContext* serviceContext) override;

	void update(World* world, WorldContext* worldContext, ServiceContext* serviceContext) override;
	std::string_view getName() override;
	ISystem* clone() override;

private:
	void changeWindowMode(ServiceContext* serviceContext);

	int anteriorWidth, anteriorHeight;
	ScreenMode anteriorScreenMode;	
};
