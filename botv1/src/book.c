#include "search.h"
#include "evaluate.h"
#include "generate.h"
#include "book.h"

struct move polyglot_to_move(uint16_t poly_move, const struct position *pos) {
    struct move best_move = {0};
    
    int from = (poly_move >> 6) & 0x3F;  // Extract 'from' square
    int to = poly_move & 0x3F;           // Extract 'to' square
    
    struct move moves[MAX_MOVES];
    size_t count = generate_legal_moves(pos, moves);

    for (size_t i = 0; i < count; i++) {
        if (moves[i].from_square == from && moves[i].to_square == to) {
            return moves[i];
        }
    }

    return best_move;  // If not found, return empty move
}

void find_book_move(const struct search_info *info, struct move* book_move)
{
    FILE* book = fopen("../Book.bin", "rb");
    if (!book)
    {
        printf("ERROR when opening book.bin");
        return ;
    }
    uint64_t position_key = compute_zobrist_hash(info->pos);
    struct book_entry entry;

    while (fread(&entry, sizeof(struct book_entry), 1, book)) {
        if (entry.key == position_key) {
            *book_move = polyglot_to_move(entry.move, info->pos);
            printf("Book move found!\n");
            fclose(book);
            return;
        }
    }

    fclose(book);
}
