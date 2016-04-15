#ifndef AI_H
#define AI_H

#include "board.h"

static const int MaxScore = 1000000;

struct SearchResult {
  int move;
  int score;
  int nodes;
};

SearchResult SearchMove(const Board *board, int player);

#endif
