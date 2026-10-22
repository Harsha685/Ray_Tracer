# Ray tracer build file
# Run these from the project root (the folder containing this Makefile):
#   make test   -> build and run src/test.cpp
#   make run    -> build src/main.cpp, render, convert to PNG
#   make view   -> same as run, then open the image
#   make clean  -> delete everything in build/

CXX      := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra
SRC      := src
BUILD    := build
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
	convert $(BUILD)/image.ppm $(BUILD)/image.png

view: run
	xdg-open $(BUILD)/image.png

clean:
	rm -rf $(BUILD)