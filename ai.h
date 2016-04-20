#ifndef AI_H
#define AI_H

#include "board.h"
#include "hash_table.h"

static const int MaxScore = 1000000;

struct SearchResult {
  int move = 9*9+1;
  int score = 0;
  int nodes = 0;
  int depth = 0;
};

SearchResult SearchMove(HashTable* table, const Board *board, int player, int time_limit);

#endif
