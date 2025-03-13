#include "uci.h"
#include "search.h"
#include "move.h"
#include "types.h"
#include "evaluate.h"
#include "generate.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <time.h>
#include "random64.h"
#include "zobrix_hash.h"

static char *get_line(FILE *stream) {
	size_t capacity = 1024;
	size_t size = 0;
	char *string = malloc(capacity);

	while (fgets(string + size, capacity - size, stream)) {
		size += strlen(string + size);

		if (string[size - 1] == '\n') {
			return string;
		}

		capacity *= 2;
		string = realloc(string, capacity);
	}

	free(string);

	return NULL;
}

static char *get_token(char *string, char *store) {
	string += strlen(string);
	*string = *store;

	while (isspace(*string)) {
		string++;
	}

	if (*string) {
		char *token = string;

		while (*string && !isspace(*string)) {
			string++;
		}

		*store = *string;
		*string = '\0';

		return token;
	}

	return NULL;
}

static void uci_position(struct position *pos, char *token, char *store) {
	token = get_token(token, store);

	if (token && !strcmp(token, "startpos")) {
		parse_position(pos, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
		token = get_token(token, store);
	} else if (token && !strcmp(token, "fen")) {
		char *fen = get_token(token, store);
		int index;

		token = fen;

		for (index = 0; token && index < 5; index++) {
			token = get_token(token, store);
		}

		if (token) {
			parse_position(pos, fen);
			token = get_token(token, store);
		}
	}

	if (token && !strcmp(token, "moves")) {
		while ((token = get_token(token, store))) {
			struct move move;
			if (parse_move(&move, token) == SUCCESS)
			{
				do_move(pos, move);
			}
		}
	}
}

struct move polyglot_to_move(uint16_t poly_move, const struct position *pos)
{
	struct move best_move = {0};

	int from = (poly_move >> 6) & 0x3F;		 // Extract 'from' square
	int to = poly_move & 0x3F;				 // Extract 'to' square
	int promotion = (poly_move >> 12) & 0xF; // Extract promotion piece type

	struct move moves[MAX_MOVES];
	size_t count = generate_legal_moves(pos, moves);

	for (size_t i = 0; i < count; i++)
	{
		if (moves[i].from_square == from && moves[i].to_square == to)
		{
			// Controleer of het een promotiezet is
			if (promotion != 0)
			{
				if (moves[i].promotion_type != promotion)
					continue; // Ga door als promotie niet matcht
			}

			return moves[i]; // Correcte zet gevonden
		}
	}

	return best_move; // Als niet gevonden, return lege move
}

void checkOpening(const struct position *pos)
{
	uint64_t hash = compute_zobrist_hash(pos);
	printf("Current position hash: %llx\n", hash);
	FILE *book = fopen("Book.bin", "rb");

	if (!book)
	{
		printf("ERROR: Could not open Book.bin\n");
		return;
	}

	struct book_entry entry;
	struct move book_move = {0}; // Initialiseer lege zet
	int found = 0;

	while (fread(&entry, sizeof(struct book_entry), 1, book))
	{
		// printf("Checking book entry: %llx\n", entry.key) ;
		if (entry.key == hash)
		{
			book_move = polyglot_to_move(entry.move, pos);
			printf("Book move found: %d -> %d\n", book_move.from_square, book_move.to_square);
			found = 1;
			break;
		}
	}

	fclose(book); // Zorg ervoor dat we het bestand altijd sluiten!

	if (!found)
	{
		printf("No book move found for this position.\n");
	}
}

static void uci_go(const struct position *pos, char *token, char *store) {
	struct search_info info;
	struct move move;
	char buffer[] = { '\0', '\0', '\0', '\0', '\0', '\0' };
	
	info.pos = pos;
	info.time[WHITE] = 0;
	info.time[BLACK] = 0;
	info.increment[WHITE] = 0;
	info.increment[BLACK] = 0;
	
	info.evaluatedPositions = 0;

	while ((token = get_token(token, store))) {
		if (!strcmp(token, "searchmoves")) {
			break;
		} else if (!strcmp(token, "ponder")) {
			continue;
		} else if (!strcmp(token, "infinite")) {
			continue;
		} else if (!strcmp(token, "wtime")) {
			token = get_token(token, store);
			info.time[WHITE] = token ? atoi(token) : 0;
		} else if (!strcmp(token, "btime")) {
			token = get_token(token, store);
			info.time[BLACK] = token ? atoi(token) : 0;
		} else if (!strcmp(token, "winc")) {
			token = get_token(token, store);
			info.increment[WHITE] = token ? atoi(token) : 0;
		} else if (!strcmp(token, "binc")) {
			token = get_token(token, store);
			info.increment[BLACK] = token ? atoi(token) : 0;
		} else {
			token = get_token(token, store);
		}

		if (!token) {
			break;
		}
	}

	// checkOpening(pos);

	clock_t starttime = clock(); 
	move = search(&info);
	clock_t endtime = clock();
	double search_time = ((double)(endtime - starttime)) / CLOCKS_PER_SEC * 1000;
	FILE *log_file = fopen("SEARCHLOGS.txt", "a");
	if (log_file)
	{
		fprintf(log_file, "Searchtime: %.2f ms\t%llu positions evaluated:\n", search_time, info.evaluatedPositions);
		fclose(log_file);
	}

	buffer[0] = "abcdefgh"[FILE(move.from_square)];
	buffer[1] = '1' + RANK(move.from_square);
	buffer[2] = "abcdefgh"[FILE(move.to_square)];
	buffer[3] = '1' + RANK(move.to_square);

	if (move.promotion_type != NO_TYPE) {
		buffer[4] = "pnbrqk"[move.promotion_type];
	}

	printf("bestmove %s\n", buffer);
}

void initZobristTable(struct position *pos)
{
	int index = 0;

	// Stukken per veld
	for (int piece = 0; piece < 12; piece++)
	{
		for (int square = 0; square < 64; square++)
		{
			pos->zobristTable[piece][square] = Random64[index++];
		}
	}

	// Rokade rechten
	for (int i = 0; i < 4; i++)
	{
		pos->castlingRights[i] = Random64[index++];
	}

	// En-passant velden
	for (int i = 0; i < 8; i++)
	{
		pos->enPassantFile[i] = Random64[index++];
	}

	// Zwart aan zet
	pos->blackToMove = Random64[index++];
}

void uci_run(const char *name, const char *author) {
	char *line;
	int quit = 0;
	struct position pos;

	initZobristTable(&pos);


	while (!quit && (line = get_line(stdin))) {
		char *token = line;
		char store = *token;

		*token = '\0';

		while ((token = get_token(token, &store))) {
			if (!strcmp(token, "quit")) {
				quit = 1;
			} else if (!strcmp(token, "uci")) {
				printf("id name %s\n", name);
				printf("id author %s\n", author);
				printf("uciok\n");
			} else if (!strcmp(token, "isready")) {
				printf("readyok\n");
			} else if (!strcmp(token, "position")) {
				uci_position(&pos, token, &store);
			} else if (!strcmp(token, "go")) {
				uci_go(&pos, token, &store);
			} else if (!strcmp(token, "setoption")) {
				break;
			} else if (!strcmp(token, "register")) {
				break;
			} else {
				continue;
			}

			break;
		}

		free(line);
		fflush(stdout);
	}
}
