#include <iostream>


#include "score_table.h"
#include "board.h"

double macro_score_table[1<<18];

double micro_win_prob[1<<18];
double micro_lose_prob[1<<18];

static const double probs[4] = {
  0, 0.5, 0.5, 0.00,
};

double ComputeMacroScore(int board) {
  int denominator = 0;

  int8_t decoded[9];
  DecodeBoard(decoded, board);

  for (int i = 0; i < 9; i++) {
    if (decoded[i] == 0) {
      denominator += 1;
    }
  }

  if (isDone(decoded, 1)) {
    return 1;
  }
  if(isDone(decoded, 2)) {
    return -1;
  }
  if(isFull(decoded)) {
    return 0;
  }

  double score = 0;
  double reciprocal = 1.0 / double(denominator);
  for (int i = 0; i < 9; i++) {
    for (int p = 1; p < 4; p++) {
      if (decoded[i] != 0) {
        continue;
      }
      int code = board | (p << (i * 2));
      score += macro_score_table[code] * reciprocal * probs[p];
    }
  }
  return score;
}

double ComputeProb(int board, int player, double* table) {
  int denominator = 0;

  int8_t decoded[9];
  DecodeBoard(decoded, board);

  for (int i = 0; i < 9; i++) {
    if (decoded[i] == 3) {
      return 0;
    }
    if (decoded[i] == 0) {
      denominator += 1;
    }
  }

  if (isDone(decoded, player)) {
    return 1;
  }
  if(isDone(decoded, 3-player) || isFull(decoded)) {
    return 0;
  }

  double score = 0;
  for (int i = 0; i < 9; i++) {
    for (int p = 1; p < 3; p++) {
      if (decoded[i] != 0) {
        continue;
      }
      int code = board | (p << (i * 2));
      score += table[code] * 0.5 / double(denominator);
    }
  }
  return score;
}

void InitScoreTable() {
  for (int i = (1<<18) - 1; i >= 0; i--) {
    macro_score_table[i] = ComputeMacroScore(i);
    micro_win_prob[i] = ComputeProb(i, 1, micro_win_prob);
    micro_lose_prob[i] = ComputeProb(i, 2, micro_lose_prob);
  }
}
