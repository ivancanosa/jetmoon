cxx = meson.get_compiler('cpp')
cc = meson.get_compiler('c')

src = [ 
'configShaderDSVariant.cpp',

'commonStructures/Navmesh.cpp',
'commonStructures/Camera.cpp',
'commonStructures/EditorConfig.cpp',

'core/GameLoop.cpp',
'core/WorldManager.cpp',
'core/Time.cpp',
'core/definitions.cpp',
'core/interpolation.cpp',
'core/SceneSerializer.cpp',
'core/WorldContextOperations.cpp',
'core/ServiceContext.cpp',
'utils/marchingSquares.cpp',
'utils/GLUtils.cpp',
'utils/Stopwatch.cpp',
'ImGuizmo/ImGuizmo.cpp',

'opengl/Framebuffer.cpp',
'opengl/Shader.cpp',
'opengl/BatchRenderer.cpp',
'opengl/PostProcessingHelper.cpp',

'services/RenderContext.cpp',
'services/ShaderLoader.cpp',
'services/Config.cpp',
'services/EventService.cpp',
'services/Renderer2D.cpp',

'systems/RenderClear.cpp',
'systems/RenderDisplay.cpp',
'systems/RenderSprite.cpp',
'systems/RenderAnimation.cpp',
'systems/TrailSystem.cpp',
'systems/Light2DSystem.cpp',
'systems/CameraSystem.cpp',
'systems/DebugPhysics.cpp',
'systems/TilemapSystem.cpp',
'systems/DebugGUI.cpp',
'systems/ParticleSystem.cpp',
'systems/Physics.cpp',
'systems/AudioSystem.cpp',
'systems/PostProcessingSystem.cpp',
'systems/VolumetricPostProcessingSystem.cpp',
'systems/ProceduralTextureSystem.cpp',
'systems/PrefabSystem.cpp',
'systems/NavmeshSystem.cpp',
'systems/SaveEntityMapSystem.cpp',


'components/Transform.cpp',

'gameEngineGUI/imguiComponents.cpp',
'gameEngineGUI/EngineStatisticsImgui.cpp',
'gameEngineGUI/Gizmos.cpp',
'gameEngineGUI/EditorHierarchyPanel.cpp',
'gameEngineGUI/EditorComponentPanel.cpp',
'gameEngineGUI/EditorFileExplorerPanel.cpp',
'gameEngineGUI/EditorTopPanel.cpp',
'gameEngineGUI/EditorSubTopPanel.cpp',
'gameEngineGUI/EditorViewPanel.cpp',


'clipper/clipper.cpp',
'polypartition/polypartition.cpp'
]

imguiSrc = ['imgui/imgui_impl_glfw.cpp',
'imgui/imgui_impl_opengl3.cpp',
'imgui/imgui.cpp',
'imgui/imgui_draw.cpp',
'imgui/imgui_tables.cpp',
'imgui/imgui_widgets.cpp',
'imgui/imgui_demo.cpp']

gl = dependency('gl')

box2d_proj = subproject('box2d')
box2d_dep = box2d_proj.get_variable('box2d_dep')

eigen_proj = subproject('eigen')
eigen_dep = eigen_proj.get_variable('eigen_dep')

glm_proj = subproject('glm')
glm_dep = glm_proj.get_variable('glm_dep')


stb_image = library('stb_image',
                        ['utils/stb_image.cpp'],
                        install : true)

json = library('json', ['utils/json.cpp'], install: true)

tileson = library('tileson', ['utils/tileson.cpp'], install: true)

pthreads = dependency('threads')
dl = cc.find_library('dl', required : false)
math = cc.find_library('m', required : false)
miniaudio = library('miniaudio',
                        ['utils/miniaudio.cpp'],
                        dependencies: [pthreads, dl, math])

host_system = host_machine.system()
if host_system == 'windows' or host_system == 'cygwin'
	glew_dep = cxx.find_library('glew32',  dirs : [meson.current_source_dir() + '/lib'], required: true)
    steam_dep = cxx.find_library('steam_api64',  dirs : [meson.current_source_dir() + '/lib'], required: true)
else
	glew_proj = subproject('glew')
	glew_dep = glew_proj.get_variable('glew_dep')
    steam_dep = cxx.find_library('steam_api',  dirs : [meson.current_source_dir() + '/lib'], required: true)
endif

glfw_proj = subproject('glfw')
glfw_dep = glfw_proj.get_variable('glfw_dep')

nuklear = library('nuklear',
                        ['utils/nuklear.cpp'],
						dependencies: [glew_dep, glfw_dep, gl],
                        install : true)




jetmoon_inc = include_directories('.')

jetmoon_lib = library('jetmoon',
  dependencies: [glew_dep, glm_dep, gl, box2d_dep, glfw_dep, steam_dep, eigen_dep],
  sources:[src, imguiSrc], 
  link_with:[stb_image, json, tileson, nuklear, miniaudio])

jetmoon_dep = declare_dependency(link_with : jetmoon_lib,
  include_directories : jetmoon_inc)

jetmoon_AllDeps = [jetmoon_dep, glew_dep, glm_dep, gl, box2d_dep, glfw_dep, steam_dep, eigen_dep]
jetmoon_AllSharedLibs = [stb_image, json, tileson, nuklear, miniaudio]
