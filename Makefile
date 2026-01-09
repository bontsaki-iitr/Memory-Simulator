CXX = g++
CXXFLAGS = -Wall -std=c++11 -I./include

SRC_DIR = src
BUILD_DIR = build
TARGET = memsim

SRCS = $(SRC_DIR)/main.cpp \
       $(SRC_DIR)/allocator/memory_manager.cpp \
       $(SRC_DIR)/cache/cache.cpp

OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: all clean
