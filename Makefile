.PHONY=all build run clean

APP_NAME=app

ifeq ($(OS),Windows_NT)
	APP_NAME+=.exe
endif

LFLAGS=-lm
FLAGS=`sdl2-config --cflags --libs`

SRC_DIR=src
INC_DIR=inc
BIN_DIR=bin

CC=g++

SRCS:=$(wildcard $(SRC_DIR)/*.cpp)
BINS:=$(SRCS:$(SRC_DIR)/%.cpp=$(BIN_DIR)/%.o)

all:
	@make clean
	@make build

build: $(BINS)
	@echo "Building..."
	@$(CC) $(LFLAGS) $(BINS) -o $(APP_NAME) $(FLAGS)

$(BIN_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo "Creating object.."
	@$(CC) -c $< -I$(INC_DIR) -o $@

run:
	@./$(APP_NAME)

clean:
	@echo "Cleaning up..."
	@rm -rvf $(BINS)
