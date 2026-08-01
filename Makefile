CXX ?= g++
AR ?= ar
PROGRAM ?= game
PROGRAM_SRC ?= programs/main.cpp

CXXFLAGS ?= -std=c++17 -Wall -Wextra -pedantic -I.
BUILD_DIR ?= build
OBJ_DIR := $(BUILD_DIR)/obj
LIB_DIR := $(BUILD_DIR)/lib
BIN_DIR := $(BUILD_DIR)/bin
LIB_NAME := $(LIB_DIR)/libasciipp_engine.a

LIB_SOURCES := \
	core/GameEngine.cpp \
	rendering/Renderer.cpp \
	utils/TerminalUtils.cpp \
	utils/ScriptUtils.cpp \
	input/InputHandler.cpp \
	systems/UIManager.cpp \
	systems/EventSystem.cpp \
	entity/Entity.cpp \
	entity/Player.cpp \
	entity/Enemy.cpp \
	world/Map.cpp

LIB_OBJECTS := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(LIB_SOURCES))
PROGRAM_OBJECT := $(OBJ_DIR)/$(PROGRAM_SRC:.cpp=.o)
PROGRAM_BINARY := $(BIN_DIR)/$(PROGRAM)

.PHONY: all library program run clean help

all: program

library: $(LIB_NAME)

program: $(PROGRAM_BINARY)

run: $(PROGRAM_BINARY)
	./$(PROGRAM_BINARY)

$(LIB_NAME): $(LIB_OBJECTS)
	@mkdir -p $(LIB_DIR)
	$(AR) rcs $@ $(LIB_OBJECTS)

$(PROGRAM_BINARY): $(PROGRAM_OBJECT) $(LIB_NAME)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(PROGRAM_OBJECT) -L$(LIB_DIR) -lasciipp_engine -o $@

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

help:
	@echo "Targets:"
	@echo "  make                 Build the default program and the engine library"
	@echo "  make library         Build libasciipp_engine.a only"
	@echo "  make program         Build the selected program"
	@echo "  make run             Build and run the selected program"
	@echo "  make clean           Remove build artifacts"
	@echo ""
	@echo "Variables:"
	@echo "  PROGRAM=<name>       Output binary name (default: game)"
	@echo "  PROGRAM_SRC=<file>   Program source file (default: programs/main.cpp)"
	@echo ""
	@echo "Examples:"
	@echo "  make"
	@echo "  make PROGRAM=my_rpg"
	@echo "  make PROGRAM=maze PROGRAM_SRC=programs/maze.cpp"
