srcGameSystems = [
'WorldCreator.cpp',
]

srcGame = [
'game.h'
]

game_inc = include_directories('.')

gameSystems_lib = library('gameSystems',
  sources: [srcGameSystems],
  dependencies: jetmoon_AllDeps,
  link_with: jetmoon_AllSharedLibs)


game_lib = library('game',
  sources: [srcGame],
  dependencies: jetmoon_AllDeps,
  link_with: [jetmoon_AllSharedLibs, gameSystems_lib], install: true)

game_dep = declare_dependency(link_with : [jetmoon_AllSharedLibs, game_lib, gameSystems_lib],
  include_directories : [jetmoon_inc, game_inc])

game_AllDeps = [game_dep, jetmoon_AllDeps]
