# Projectnaam
NAME := chessbot

# Compilers en flags
CC := gcc
CXX := g++
CFLAGS := -Wall -Wextra -pedantic -std=c99
CXXFLAGS := -Wall -Wextra -pedantic -std=c++11

# Mappen
SRC_DIR := src
BUILD_DIR := build
INCLUDE_DIR := include

# Headerbestanden
HEADERS := $(wildcard $(INCLUDE_DIR)/*.h)

# Bronbestanden
SRC_C := $(wildcard $(SRC_DIR)/*.c)
SRC_CPP := $(wildcard $(SRC_DIR)/*.cpp)

# Objectbestanden
OBJ_C := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRC_C))
OBJ_CPP := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRC_CPP))

# Targets
all: $(NAME)

# Maak de build-map als die niet bestaat
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compileer C-bestanden
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS) Makefile | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@ -I$(INCLUDE_DIR)

# Compileer C++-bestanden
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS) Makefile | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@ -I$(INCLUDE_DIR)

# Link het eindprogramma
$(NAME): $(OBJ_C) $(OBJ_CPP)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Opschonen
clean:
	rm -rf $(BUILD_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

# Run het chessbot script
