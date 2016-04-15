#include "ai.h"

#include <algorithm>
#include <cstdlib>

int leafEval(const Board *board, int player) {
  int out = 0;
  int other = 3-player;
  for (auto c : board->macrocells) {
    if (c == player) {
      out ++;
    } else if (c == other) {
      out --;
    }
  }
  return out;
}

SearchResult DeepEval(const Board *board, int player, int depth, int alpha, int beta) {
  SearchResult out;
  out.score = -MaxScore;
  out.nodes = 1;

  if (board->isOver()) {
    out.score = -MaxScore - depth + 100;
    return out;
  }

  if (board->isDrawn()) {
    out.score = 0;
    return out;
  }

  if (depth == 0) {
    out.score = leafEval(board, player);
    return out;
  }

  for (int cell = 0; cell < 9*9; cell++) {
    if (!board->canTick(cell)) {
      continue;
    }
    Board copy = *board;
    copy.tick(cell, player);
    SearchResult child = DeepEval(&copy, 3-player, depth-1, -beta, -alpha);
    int score = -child.score;
    out.nodes += child.nodes;
    if (score > out.score) {
      out.score = score;
      alpha = max(alpha, score+1);
      if (score > beta) {
        break;
      }
    }
  }

  return out;
}

static const int InitialDepth = 10;

SearchResult SearchMove(const Board *board, int player) {
  SearchResult out;
  out.score = -MaxScore;
  out.move = -1;
  out.nodes = 1;

  int alternatives[9*9];
  int alternative_count = 0;

  for (int cell = 0; cell < 9*9; cell++) {
    if (!board->canTick(cell)) {
      continue;
    }
    Board copy = *board;
    copy.tick(cell, player);
    SearchResult child = DeepEval(&copy, 3-player, InitialDepth-1, -MaxScore, -out.score);
    int score = -child.score;
    out.nodes += child.nodes;
    if (score > out.score || out.move == -1) {
      out.score = score;
      out.move = cell;
      alternatives[0] = cell;
      alternative_count = 1;
    } else if (score == out.score) {
      alternatives[alternative_count] = cell;
      alternative_count++;
    }
  }

  if (alternative_count > 1) {
    out.move = alternatives[rand() % alternative_count];
  }

  return out;
}
