#include "WorldContext.hpp"
#include <memory>

std::shared_ptr<WorldContext> createWorldContext(){
	return std::make_shared<WorldContext>();
}
std::shared_ptr<WorldContext> cloneWorldContext(std::shared_ptr<WorldContext> worldContext){
	return std::make_shared<WorldContext>(*worldContext);
}
