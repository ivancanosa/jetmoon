#pragma once

#include <memory>
struct WorldContext;

std::shared_ptr<WorldContext> createWorldContext();
std::shared_ptr<WorldContext> cloneWorldContext(std::shared_ptr<WorldContext> worldContext);

