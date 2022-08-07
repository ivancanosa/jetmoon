#pragma once
#include "core/definitions.hpp"  // for Entity
class World;
struct ServiceContext;

class EditorComponentPanel{
public:
	void render(World* world, ServiceContext* serviceContext, Entity entity);

};
