#pragma once

#include <fstream>                 // for string
#include <string>                  // for basic_string
#include <vector>                  // for vector
#include "utils/json.hpp"          // for basic_json, NLOHMANN_DEFINE_TYPE_N...
#include "utils/visit_struct.hpp"  // for accessor, VISIT_STRUCT_APPLYF2

static const std::string editorConfigPath{".editorData.json"};

struct EditorConfig{
	bool allowAutosave{true};
	int autosaveTime{180};//in seconds
	std::string lastOpenProyect{""};
	std::vector<std::string> lastOpenProyects{};

	void save();
	void load();
};




VISITABLE_STRUCT(EditorConfig, allowAutosave, autosaveTime);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(EditorConfig, autosaveTime, allowAutosave, lastOpenProyect, lastOpenProyects);
