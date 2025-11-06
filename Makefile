CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2 -I./src
LDFLAGS = -lSDL2 -lm

SRC_DIR = src
TARGET = three_renderer

SOURCES = $(SRC_DIR)/main.cpp
HEADERS = $(SRC_DIR)/vector3.hpp $(SRC_DIR)/plane.hpp $(SRC_DIR)/space.hpp $(SRC_DIR)/viewpoint.hpp

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
