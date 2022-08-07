#pragma once

#include <memory>
#include <string>
class World;

std::shared_ptr<World> createPlayableWorld(std::string scenePath="");
