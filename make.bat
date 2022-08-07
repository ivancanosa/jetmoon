@echo off
set BUILDDIR=builddir
set MAIN=main
set RELEASE=releasebuild
set TARGET=%BUILDDIR%\%MAIN%
set TARGET_RELEASE=%RELEASE%\%MAIN%

set CXXFLAGS=-stdlib=libc++
set CXX=clang++
set CC=clang

if "%1%" == ""       	  goto build
if "%1%" == "build"  	  goto build
if "%1%" == "run"   	  goto run
if "%1%" == "clean"   	  goto clean
if "%1%" == "release"  	  goto release
if "%1%" == "runRelease"  goto runRelease
echo "Command no valid"
exit /b

:build
	if not exist %BUILDDIR% meson setup %BUILDDIR% --native-file .\debugConfig
	meson configure %BUILDDIR% -DisRelease=false
	ninja  -C %BUILDDIR% && cp %BUILDDIR%/compile_commands.json compile_commands.json
	xcopy /s /y lib %BUILDDIR%
	exit /b

:run
	if not exist %BUILDDIR% meson setup %BUILDDIR% --native-file debugConfig
	meson configure %BUILDDIR% -DisRelease=false
	ninja  -C %BUILDDIR% && cp %BUILDDIR%/compile_commands.json compile_commands.json
	xcopy /s /y lib %BUILDDIR%
	%TARGET%.exe
	exit /b

:release
	if not exist %RELEASE% meson setup %RELEASE% --native-file .\debugConfig
	meson configure %RELEASE% -DisRelease=true
	ninja  -C %RELEASE%
	mkdir release
	xcopy /s /y lib release
	mkdir release\data
	xcopy data release\data /s/h/e/k/f/c/y
	copy %RELEASE%\main.exe release /y
	exit /b

:runRelease
	if not exist %RELEASE% meson setup %RELEASE% --native-file .\debugConfig
	meson configure %RELEASE% -DisRelease=true
	ninja  -C %RELEASE%
	mkdir release
	xcopy /s /y lib release
	mkdir release\data
	xcopy data release\data /s/h/e/k/f/c/y
	copy %RELEASE%\main.exe release /y
	release\main.exe
	exit /b


:clean
	rmdir /s /q %BUILDDIR%
	rmdir /s /q %RELEASE%
	rmdir /s /q release
	rmdir /s /q compile_commands.json 
	exit /b