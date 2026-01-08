# Compiler settings
CXX = g++
CXXFLAGS = -Wall -std=c++11 -I./include

# Directories
SRC_DIR = src
BUILD_DIR = build
TARGET = memsim

# Source files (Phase 2 Only - No Virtual Memory)
SRCS = $(SRC_DIR)/main.cpp \
       $(SRC_DIR)/allocator/memory_manager.cpp \
       $(SRC_DIR)/cache/cache.cpp

# Object files (Automatically creates a list of .o files from .cpp files)
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# 1. The Default Goal: Build the target executable
all: $(TARGET)

# 2. How to link the executable from the object files
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# 3. How to compile each .cpp file into a .o file
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 4. Cleanup command (run 'make clean' to delete built files)
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: all clean