#pragma once

#include <assert.h>
#include <tuple>
#include <vector>
#include <unordered_map>
#include <map>

#include <GLFW/glfw3.h>
#include "utils/nuklear.h"
#include "utils/nuklear_glfw_gl4.h"


#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024



class NuklearService{
public:
	struct nk_context *ctx;

	void setFontRange(int minSize, int maxSize){
		assert(minSize < maxSize && minSize > 5 && "The font size range is incorrect");
		this->minSize = minSize;
		this->maxSize = maxSize;
	}

	void addFontToLoad(std::string id, std::string path){
		fontsToLoadVector.push_back({id, path});
	}

	void setFont(std::string id, int size){
		size = std::clamp(size, minSize, maxSize);
		auto font = fontsMap[{id, size}];
		nk_style_set_font(ctx, &font->handle);
	}

	void init(GLFWwindow* window){
		ctx = nk_glfw3_init(window, NK_GLFW3_INSTALL_CALLBACKS, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);

		struct nk_font_atlas *atlas;
		nk_glfw3_font_stash_begin(&atlas);

		for(auto[id, path]: fontsToLoadVector){
			for(int i=minSize; i<=maxSize; i++){
				fontsMap[{id, i}] = nk_font_atlas_add_from_file(atlas, path.c_str(), i, 0);
			}
		}

		nk_glfw3_font_stash_end();
	}

private:
	std::vector<std::tuple<std::string, std::string>> fontsToLoadVector{};
	std::map<std::tuple<std::string, int>, nk_font*> fontsMap;
	int minSize{10}, maxSize{40};
};
