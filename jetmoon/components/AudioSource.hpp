#pragma once

#include "utils/ComponentInspector.hpp"
#include "commonStructures/ConfigDS.hpp"

struct AudioSource{
	std::string audioId{""};
	AudioGroup audioGroup{AudioGroup::SfxGroup};
	float volume{1.};
	float pitch{1.};
	bool playInLoop{false};
	bool isPositional{false};

	bool play{false};
	bool reset{false};
	// Auxiliary members
	bool isPlaying{false};
};

COMPONENT_INSPECTOR(AudioSource, audioId, audioGroup, volume, play, playInLoop, reset, isPositional);
