#pragma once
#include "utils/ComponentInspector.hpp"
#include "core/definitions.hpp"

struct LightningConfig{
	Color3 backgroundColor{1., 1., 1.};
	bool lightMask{false};
};

COMPONENT_INSPECTOR(LightningConfig, backgroundColor, lightMask);
