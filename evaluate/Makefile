# Project Name
NAME := evaluate

# Compiler & Flags
CC     := gcc
CFLAGS := -Wall -Wextra -pedantic

# Directories
SRC_DIR  := src
BUILD_DIR := build
INCLUDE_DIR := include

# Find all source files in SRC_DIR
SRCS := $(wildcard $(SRC_DIR)/*.c)

# Generate corresponding object files in BUILD_DIR
OBJS := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

# Headers
HEADERS := $(wildcard $(INCLUDE_DIR)/*.h)

# Build rules
$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Ensure build directory exists
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Clean rules
clean:
	rm -rf $(BUILD_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean $(NAME)

restart: re
	./update_engine.bat  # Call the script after recompiling

start: 
	./update_engine.bat  # Call the script after recompiling
