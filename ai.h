#ifndef AI_H
#define AI_H

#include "board.h"

struct SearchResult {
  int move;
  int score;
};

SearchResult SearchMove(Board *board, int player, int depth = 2);

#endif
