# Ray tracer build file
# Run these from the project root (the folder containing this Makefile):
#   make test   -> build and run src/test.cpp
#   make run    -> build src/main.cpp, render, save a timestamped PNG in renders/
#   make view   -> same as run, then open the newest image in renders/
#   make clean  -> delete everything in build/

CXX      := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra
SRC      := src
BUILD    := build
RENDERS  := renders
HEADERS  := $(wildcard $(SRC)/*.h)

.PHONY: all run view test clean

all: $(BUILD)/raytracer

$(BUILD):
	mkdir -p $(BUILD)

# Rebuilds automatically when main.cpp or any .h file changes
$(BUILD)/raytracer: $(SRC)/main.cpp $(HEADERS) | $(BUILD)
	$(CXX) $(CXXFLAGS) $(SRC)/main.cpp -o $@

$(BUILD)/test: $(SRC)/test.cpp $(HEADERS) | $(BUILD)
	$(CXX) $(CXXFLAGS) $(SRC)/test.cpp -o $@

test: $(BUILD)/test
	./$(BUILD)/test

run: $(BUILD)/raytracer
	./$(BUILD)/raytracer > $(BUILD)/image.ppm
	mkdir -p $(RENDERS)
	convert $(BUILD)/image.ppm $(RENDERS)/render_$$(date +%Y%m%d_%H%M%S).png

view: run
	xdg-open "$$(ls -t $(RENDERS)/*.png | head -n 1)"

clean:
	rm -rf $(BUILD)