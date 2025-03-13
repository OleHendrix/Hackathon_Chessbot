NAME := chessbot
CC := gcc
CXX := g++
CFLAGS := -Wall -Wextra -pedantic -std=c99
CXXFLAGS := -Wall -Wextra -std=c++11

HEADERS := include/uci.h include/perft.h include/search.h include/evaluate.h \
           include/generate.h include/move.h include/position.h include/parse.h include/types.h include/zobrix_hash.h include/random64.h

SRC_C := src/uci.c src/perft.c src/search.c src/evaluate.c src/generate.c src/move.c src/position.c src/parse.c src/main.c
SRC_CPP := src/zobrix_hash.cpp 

OBJ_C := $(SRC_C:src/%.c=build/%.o)
OBJ_CPP := $(SRC_CPP:src/%.cpp=build/%.o)

all: $(NAME)

build/%.o: src/%.c $(HEADERS) Makefile
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@ -Iinclude

build/%.o: src/%.cpp $(HEADERS) Makefile
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@ -Iinclude

$(NAME): $(OBJ_C) $(OBJ_CPP)
	$(CXX) $(CXXFLAGS) $^ -o $@  # Gebruik g++ voor linking

clean:
	rm -rf build/

fclean:
	rm -rf build/
	rm -f $(NAME)

re:
	${MAKE} fclean
	${MAKE} run

run:
	./start_chessbot.sh