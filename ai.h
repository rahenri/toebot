#ifndef AI_H
#define AI_H

#include <iostream>

#include "board.h"
#include "hash_table.h"

struct SearchResult {
  int moves[9*9];
  int move_count = 0;
  int score = 0;
  int64_t nodes = 0;
  int depth = 0;

  int RandomMove() const;
};

SearchResult SearchMove(HashTable* table, const Board *board, int player, int time_limit, bool use_open_table = true);

std::ostream& operator<<(std::ostream&, const SearchResult&);

#endif
