BUILDDIR=builddir
MAIN=main
RELEASE=releasebuild

TARGET=$(BUILDDIR)/$(MAIN)

.PHONY: build debug run clean profile release

export CXXFLAGS=-stdlib=libc++
export CXX=clang++
export CC=clang

all: build

build: $(BUILDDIR)
	ninja  -C $(BUILDDIR) && cp $(BUILDDIR)/compile_commands.json compile_commands.json

releaseBuild: $(RELEASE)
	ninja  -C $(RELEASE)

debug: $(BUILDDIR)
	ninja  -C $(BUILDDIR) && cp $(BUILDDIR)/compile_commands.json compile_commands.json
	gdb -batch -ex "run" -ex "bt" $(TARGET) 2>&1 | grep -v ^"No stack."$

run: $(BUILDDIR)
	ninja  -C $(BUILDDIR) && cp $(BUILDDIR)/compile_commands.json compile_commands.json
	./$(TARGET)

release: $(RELEASE)
	ninja  -C $(RELEASE)
	mkdir -p release
	cp -r $(RELEASE)/main release/main
	cp -r data release

runRelease: $(RELEASE)
	ninja  -C $(RELEASE)
	mkdir -p release
	cp -r $(RELEASE)/main release/main
	cp -r data release
	./release/main

$(BUILDDIR): meson.build
	meson setup $(BUILDDIR) --native-file debugConfig
	meson configure $(BUILDDIR) -DisRelease=false

$(RELEASE): meson.build
	meson setup $(RELEASE) --native-file debugConfig
	meson configure $(RELEASE) -DisRelease=true

clean:
	rm -rf $(BUILDDIR)
	rm -rf $(RELEASE)
	rm -rf release
	rm -f compile_commands.json 
	 
profile: $(BUILDDIR)
	CPUPROFILE=prof.out ./$(TARGET)
	pprof --gif $(TARGET) prof.out > output.gif && xdg-open output.gif

record: $(BUILDDIR)
	perf record ./$(TARGET) 
	
report: $(BUILDDIR)
	perf report

headers: $(BUILDDIR)
	python iwyu_tool.py -p . > log

