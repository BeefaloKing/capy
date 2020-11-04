# Name of target executable
BIN = capy.exe

# List of source files to be compiled
FILES = main.cpp automata.cpp error.cpp storage.cpp

# Compiler to use
CC = g++

# Compiler flags for debug/release builds
CXXFLAGS_DEBUG = -g -O0 -Wall
CXXFLAGS_RELEASE = -DNDEBUG -O2

# Libraries to link against
LDLIBS =

# Linker flags for debug/release builds
LDFLAGS_DEBUG = -static
LDFLAGS_RELEASE = -Wl,-O,--gc-sections,--strip-all -static

# Directories to search for includes
# Separate from CXXFLAGS so user can provide custom flags
INCLUDE = -Iinclude

ifeq ($(filter MINGW32 MINGW64,$(MSYSTEM)),)
$(error Please build with either MinGW 32 or MinGW 64)
endif

# Find full list of subdirectories in the source tree
# These are mirrored in the object and dependency trees
FSUBDIRS = $(shell dirname $(FILES))
FSUBDIRS := $(filter-out .,$(FSUBDIRS))
FSUBDIRS := $(sort $(FSUBDIRS))

.SECONDEXPANSION:
.PHONY: all build debug release

all: debug

# Set custom BUILD_TYPE if user provides custom flags
ifeq ($(CXXFLAGS)$(LDFLAGS),)
debug: BUILD_TYPE = debug
release: BUILD_TYPE = release
else
debug release: BUILD_TYPE = custom
endif

# Use default CXXFLAGS if user provides none
ifeq ($(CXXFLAGS),)
debug: export CXXFLAGS = $(CXXFLAGS_DEBUG)
release: export CXXFLAGS = $(CXXFLAGS_RELEASE)
endif

# Use default LDFLAGS if user provides none
ifeq ($(LDFLAGS),)
debug: export LDFLAGS = $(LDFLAGS_DEBUG)
release: export LDFLAGS = $(LDFLAGS_RELEASE)
endif

# Ensure target directories exist and invoke recursive make
debug release: export BIN_PATH = $(MSYSTEM_CARCH)/$(BUILD_TYPE)
debug release: export OBJ_PATH = $(MSYSTEM_CARCH)/$(BUILD_TYPE)/obj
debug release: export DEP_PATH = $(MSYSTEM_CARCH)/$(BUILD_TYPE)/dep
debug release:
	@mkdir -p bin/$(BIN_PATH)
	@mkdir -p bld/$(OBJ_PATH) $(patsubst %,bld/$(OBJ_PATH)/%,$(FSUBDIRS))
	@mkdir -p bld/$(DEP_PATH) $(patsubst %,bld/$(DEP_PATH)/%,$(FSUBDIRS))
	@echo "Building \"$(BUILD_TYPE)\" for $(MSYSTEM_CARCH)"
	@$(MAKE) build

clean:
	rm -rf bld

clean-all:
	rm -rf bld
	rm -rf bin

build: bin/$(BIN_PATH)/$(BIN)

# Finish linking
bin/$(BIN_PATH)/$(BIN): OBJECTS = $(patsubst %.cpp,bld/$(OBJ_PATH)/%.o,$(FILES))
bin/$(BIN_PATH)/$(BIN): $$(OBJECTS)
	$(CC) $^ $(CXXFLAGS) $(LDLIBS) $(LDFLAGS) -o $@

# Generate dependency files and compile objects
bld/$(OBJ_PATH)/%.o: src/%.cpp bld/$(DEP_PATH)/%.d
	$(CC) -MMD -MP -MF bld/$(DEP_PATH)/$*.d -c $< $(CXXFLAGS) $(INCLUDE) -o $@

# Include rules for generated dependencies
ifdef DEP_PATH
DEP_FILES = $(patsubst %.cpp,bld/$(DEP_PATH)/%.d,$(FILES))
$(DEP_FILES):
include $(wildcard $(DEP_FILES))
endif
