#ifndef AI_H
#define AI_H

#include <iostream>

#include "board.h"

struct SearchResult {
  int moves[9*9];
  int move_count = 0;
  int score = 0;
  int64_t nodes = 0;
  int depth = 0;
  bool time_limit_exceeded = false;
  bool manual_interruption = false;

  int RandomMove() const;
};


struct SearchOptions {
  bool use_open_table = true;
  bool interruptable = false;
  int time_limit = 100;
};

SearchResult SearchMove(const Board *board, int player, SearchOptions opt = SearchOptions());

std::ostream& operator<<(std::ostream&, const SearchResult&);

#endif
