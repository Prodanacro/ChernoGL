# ------------------------- GENERAL PROJECT SETTINGS ---------------------------
EXECUTABLE = Cherno
CXX = g++

# Directory structure.
INCLUDE_DIR = include
SRC_DIR = src
OBJ_DIR = obj
LIB_DIR = lib
# ------------------------------------------------------------------------------


# ------------------------- COMPILER AND LINKER FLAGS --------------------------
# Debugging flags. Remove if not debugging to enable optimisations. 
DEBUG_FLAGS = -ggdb -O0 -DDEBUG

# Compiler flags.
CXXFLAGS = -x c++ -Wall -pedantic-errors -O2 -I$(INCLUDE_DIR) $(DEBUG_FLAGS)

# Linker flags and libraries to link against.
#LDFLAGS =
LDLIBS = -lGL -lGLU -lGLEW -lglfw

# Link against project specific libraries in 'lib' directory.
LDFLAGS += -Llib
# ------------------------------------------------------------------------------


# ---------------------------- FILES AND PHONY RULES ---------------------------
# All source (object) and headers files.
SRC = $(wildcard $(SRC_DIR)/*.cpp)
OBJ = $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Special words. Ignore files with those names.
.PHONY: all clean
# ------------------------------------------------------------------------------


# ------------------------------- DEFAULT TARGET -------------------------------
# Build the executable. 
all: $(EXECUTABLE)

# Linking.
$(EXECUTABLE): $(OBJ)
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@ 
	
# Compiling.
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
# ------------------------------------------------------------------------------


# --------------------------------- CLEANING -----------------------------------
# Remove object files - 'obj' directory is not deleted.
clean:
	$(RM) $(OBJ)
# ------------------------------------------------------------------------------
