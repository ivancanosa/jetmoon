project('jetmoon', 'cpp', 'c', default_options: ['default_library=static'])

add_global_arguments('-std=c++17', language : 'cpp')
add_global_arguments('-stdlib=libc++', language : 'cpp')
add_global_arguments('-Wno-everything', language : 'cpp')
#add_global_arguments('-g', language : 'cpp')
#add_global_arguments('-O0', language : 'cpp')
#add_global_arguments('-fmodules', language : 'cpp')

#add_global_arguments('-ftime-report', language : 'cpp')
#add_global_arguments('-ftime-trace', language : 'cpp')

if get_option('isRelease')
	add_global_arguments('-D __RELEASE__', language : 'cpp')
endif


subdir('jetmoon')
subdir('game')

executable('main', 'main.cpp',
  dependencies: game_AllDeps)
