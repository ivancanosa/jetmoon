#pragma once
#include <memory>
#include <iosfwd>                                     // for string
#include "GL/glew.h"                                  // for GLint, GLuint
#include "core/ISystem.hpp"                           // for ISystem
#include "core/definitions.hpp"                       // for Vec2
#include "gameEngineGUI/EditorComponentPanel.hpp"     // for EditorComponent...
#include "gameEngineGUI/EditorFileExplorerPanel.hpp"  // for EditorFileExplo...
#include "gameEngineGUI/EditorHierarchyPanel.hpp"     // for EditorHierarchy...
#include "gameEngineGUI/EditorSubTopPanel.hpp"        // for EditorSubTopPanel
#include "gameEngineGUI/EditorTopPanel.hpp"           // for EditorTopPanel
#include "gameEngineGUI/EditorViewPanel.hpp"          // for EditorViewPanel
class IDialogStructEditor;
class World;
struct ImGuiContext;
struct ServiceContext;
struct WorldContext;


class DebugGUI: public ISystem{
public:
	void init(World* world, WorldContext* worldContext, ServiceContext* serviceContext) override;
	void update(World* world, WorldContext* worldContext, ServiceContext* serviceContext) override;
	ISystem* clone() override;
	~DebugGUI();

private:
	void newFrame();
	void render();
	void renderCameraFrame(World* world, WorldContext* worldContext, ServiceContext* serviceContext);
	void renderGrid(World* world, WorldContext* worldContext, ServiceContext* serviceContext);

	ImGuiContext* ctx{nullptr};

	EditorHierarchyPanel editorHierarchyPanel{};
	EditorComponentPanel editorComponentPanel{};
	EditorFileExplorerPanel editorFileExplorerPanel{};
	EditorTopPanel editorTopPanel;
	EditorViewPanel editorViewPanel;
	EditorSubTopPanel editorSubTopPanel;

	Vec2 viewMargin{1., 1.}, viewSize{1., 1.};

	const std::string editorConfigPath{".editorData.json"};
	std::shared_ptr<IDialogStructEditor> dialogStructEditor{nullptr};
	GLuint texture;
	GLint windowFramebuffer{0}, framebuffer{0};
};
