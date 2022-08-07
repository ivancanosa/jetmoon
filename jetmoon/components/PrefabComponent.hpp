#pragma once
#include "utils/ComponentInspector.hpp"
#include "core/definitions.hpp"
#include <unordered_set>

struct PrefabComponent{
	std::string prefab{""};
	bool load{true};
	std::unordered_set<std::string> changedComponents{};
};

VISITABLE_STRUCT(PrefabComponent, prefab);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(PrefabComponent, prefab, changedComponents);
