NAME := chessbot

CC := gcc
CXX := g++
CFLAGS := -Wall -Wextra -pedantic -std=c99
CXXFLAGS := -Wall -Wextra -pedantic -std=c++11

SRC_DIR := src
BUILD_DIR := build
INCLUDE_DIR := include

HEADERS := $(wildcard $(INCLUDE_DIR)/*.h)

SRC_C := $(wildcard $(SRC_DIR)/*.c)
SRC_CPP := $(wildcard $(SRC_DIR)/*.cpp)

OBJ_C := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRC_C))
OBJ_CPP := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRC_CPP))

all: $(NAME)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS) Makefile | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@ -I$(INCLUDE_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS) Makefile | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@ -I$(INCLUDE_DIR)

$(NAME): $(OBJ_C) $(OBJ_CPP)
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	rm -rf $(BUILD_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all
