#pragma once
#include <string>               // for string
#include "utils/Stopwatch.hpp"  // for Stopwatch
class World;
struct EditorConfig;
struct ServiceContext;
struct WorldContext;

class EditorTopPanel{
public:
	bool render(World* world, WorldContext* worldContext, ServiceContext* serviceContext, int width, int height);
	
private:
	void newAsDialog(World* world, ServiceContext* serviceContext);
	void openAsDialog(World* world, ServiceContext* serviceContext);
	void saveAsDialog(World* world, ServiceContext* serviceContext);
	void editPreferencesDialog(World* world, ServiceContext* serviceContext);

	void saveCurrentWorld(World* world);

	void updateOpenProyectsVector();

	bool openAsDialogCond{false}, saveAsDialogCond{false}, newDialogCond{false}, editPreferencesCond{false};
	bool isDialogOpen{false};
	std::string scenePath{""};
	bool resetScene{false};
	Stopwatch stopwatch{};

	EditorConfig* editorConfig{nullptr};
};
