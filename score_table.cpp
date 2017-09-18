#include <iostream>


#include "score_table.h"
#include "board.h"

double macro_score_table[1<<18];

int8_t best_cell_table[2][1<<18];

double micro_win_prob[1<<18];
double micro_lose_prob[1<<18];

int reg_cell_score_int[9*9];
int reg_cell_bias_int;

extern double cell_score [9*9];
extern double reg_cell_bias;

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

int ComputeBestCell(int board, int player) {
  int best_cell = -1;
  double best = -1.0e9;
  for (int i = 0; i < 9; i++) {
    int cell = (board >> (i * 2)) & 3;
    if (cell == 3) {
      return -1;
    }
    if (cell != 0) {
      continue;
    }
    int b2 = board | (player << (i * 2));
    double score = micro_win_prob[b2] - micro_lose_prob[b2];
    if (player == 2) score = -score;
    if (score > best) {
      best = score;
      best_cell = i;
    }
  }
  return best_cell;
}

void InitScoreTable() {
  for (int i = (1<<18) - 1; i >= 0; i--) {
    macro_score_table[i] = ComputeMacroScore(i);
    micro_win_prob[i] = ComputeProb(i, 1, micro_win_prob);
    micro_lose_prob[i] = ComputeProb(i, 2, micro_lose_prob);
  }
  for (int p = 1; p < 3; p++) {
    for (int i = 0; i < 1<<18; i++) {
      best_cell_table[p-1][i] = ComputeBestCell(i, p);
    }
  }

  for (int i = 0; i < 9*9; i++) {
    reg_cell_score_int[i] = int(cell_score[i] * score_factor + 0.5);
  }
  reg_cell_bias_int = int(reg_cell_bias * score_factor + 0.5);
}
