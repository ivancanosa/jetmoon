#pragma once
#include "utils/nuklear.h"
#include "utils/nuklear_glfw_gl4.h"

struct FloatRect{
	float width{0.5}, height{0.5};
	float marginX{0.}, marginY{0.};
	int horizontalFloat{0};
	int verticalFloat{0};

	auto getRect(int viewportWidth, int viewportHeight){
		int dx{0}, dy{0};
		switch(horizontalFloat){
			case -1:
				dx = viewportWidth*marginX;
				break;
			case 0:
				dx = viewportWidth/2. - viewportWidth*width/2. + viewportWidth*marginX;
				break;
			case 1:
				dx = viewportWidth - viewportWidth*width - viewportWidth*marginX;
				break;
		}

		switch(verticalFloat){
			case -1:
				dy = viewportHeight*marginY;
				break;
			case 0:
				dy = viewportHeight/2. - viewportHeight*height/2. + viewportHeight*marginY;
				break;
			case 1:
				dy = viewportHeight - viewportHeight*height - viewportHeight*marginY;
				break;
		}
		return nk_rect(dx, dy, width*viewportWidth, height*viewportHeight);
	}
};
