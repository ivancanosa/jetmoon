#include "ServiceContext.hpp"
#include "services/ImageLoader.hpp"
#include "services/AnimationDefinitionService.hpp"
#include "services/ShaderLoader.hpp"

void ServiceContext::hotReload(){
	imageLoader->hotReload();
	animationDefinitionService->hotReload();
	shaderLoader->hotReload();
}
