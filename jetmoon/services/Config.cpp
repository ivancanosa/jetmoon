#include "Config.hpp"
#include <iosfwd>  // for string
#include <string>  // for operator<
#include <tuple>   // for tuple

std::map<std::string, std::tuple<int, int>> GraphicsConfig::legalResolutions = {
	{"640×480p", {640, 480}},
	{"1280×720", {1280, 720}},
	{"1920x1080", {1920, 1080}},
	{"2560x1440", {2560, 1440}},
	{"3840x2160", {3840, 2160}},
	{"7680×4320", {7680, 4320}}
};

std::map<std::string, ScreenMode> GraphicsConfig::legalScreenModes = {
	{"Windowed", ScreenMode::Windowed},
	{"Fullscreen", ScreenMode::Fullscreen},
	{"Borderless", ScreenMode::Borderless}
};
