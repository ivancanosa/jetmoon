#pragma once

#include "utils/visit_struct.hpp"
#include "utils/json.hpp"

#define COMPONENT_INSPECTOR_VISITABLE(Type, ...) \
	VISITABLE_STRUCT(Type, __VA_ARGS__);\

#define COMPONENT_INSPECTOR_SERIALIZABLE(Type, ...) \
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Type, __VA_ARGS__);

#define COMPONENT_INSPECTOR(Type, ...) \
	VISITABLE_STRUCT(Type, __VA_ARGS__);\
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Type, __VA_ARGS__);
	/*
    inline void to_json(nlohmann::json& nlohmann_json_j, const Type& nlohmann_json_t) { NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_TO, __VA_ARGS__)) } \
    inline void from_json(const nlohmann::json& j, Type& p) {\
	  visit_struct::for_each(p,\
		[&](const char * name, auto& value) {\
			if(j.contains(name)){\
				j.at(name).get_to(value);\
			}\
		});\
    }\*/

