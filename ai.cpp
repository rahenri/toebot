#include "ai.h"

static const int MaxScore = 1000000;

int leafEval(Board *board, int player) {
  int out = 0;
  int other = 3-player;
  for (int i = 0; i < 9; i++) {
    auto c = board->macrocells[i];
    if (c == player) {
      out ++;
    } else if (c == other) {
      out --;
    }
  }
  return out;
}

SearchResult SearchMove(Board *board, int player, int depth) {
  SearchResult out;
  out.score = -MaxScore;
  out.move = -1;

  if (board->isOver()) {
    return out;
  }

  if (depth == 0) {
    out.score = leafEval(board, player);
    return out;
  }

  for (int i = 0; i < 9*9; i++) {
    if (!board->canTick(i)) {
      continue;
    }
    Board copy = *board;
    copy.tick(i, player);
    int score = -SearchMove(&copy, 3-player, depth-1).score;
    if (score > out.score || out.move == -1) {
      out.score = score;
      out.move = i;
    }
  }
  return out;
}
