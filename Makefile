# ---
# General infos
# ---

APP_NAME=app

# ---
# Build tools and flags
# ---

CC=g++
GPU_CC:=

LFLAGS:=-lm
FLAGS=`sdl2-config --cflags --libs`

NVCC_RESULT:=$(shell which nvcc 2> /dev/null)
NVCC:=$(notdir $(NVCC_RESULT))

ifeq ($(NVCC_TEST),nvcc)
	GPU_CC:=nvcc
else
	GPU_CC:=g++
	LFLAGS:=-lOpenCL $(LFLAGS)
endif

# ---
# Directories and sources
# ---

SRC_DIR=src
INC_DIR=inc
BIN_DIR=bin

SRCS:=$(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(SRC_DIR)/**/*.cpp)
BINS:=$(SRCS:$(SRC_DIR)/%.cpp=$(BIN_DIR)/%.o)

# ---
# General targets
# ---

# Default target
all: build

# Build target
build: $(APP_NAME)

# Clean target
clean:
	@rm -rf $(BINS)

# Rebuild everything
re: clean build

# Run the app
run: $(APP_NAME)
	@./$(APP_NAME)


.PHONY: all build clean re run 

# ---
# Build targets and rules
# ---

# Linking the app
$(APP_NAME): $(BINS)
	@echo "Linking..."
	$(CC) $(BINS) -o $(APP_NAME) $(LFLAGS) $(FLAGS)

# Rule for make each objects files
$(BIN_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	@echo "$(CC) $<..."
	@$(CC) -c $< -I$(INC_DIR) -o $@
