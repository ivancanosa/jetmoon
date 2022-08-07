#pragma once

#include "utils/json.hpp"
#include "utils/ComponentInspector.hpp"


struct NameComponent{
	std::string name{"NewEntity"};
};

COMPONENT_INSPECTOR(NameComponent, name);
