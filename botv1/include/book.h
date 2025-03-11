
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>

struct book_entry {
    uint64_t key;  // Zobrist hash of position (8 bytes)
    uint16_t move; // Encoded move (2 bytes)
    uint16_t weight; // Frequency (2 bytes)
    uint32_t learn;  // Learning value (4 bytes)
};

struct move polyglot_to_move(uint16_t poly_move, const struct position *pos);
void find_book_move(const struct search_info *info, struct move* book_move);
