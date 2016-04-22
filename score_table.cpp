#include "score_table.h"
#include "board.h"

ScoreCell score_lookup_table[1<<18];

static const double probs[4] = {
  0, 0.5, 0.5, 0.00,
};

double ComputeRecursive(int board) {
  ScoreCell& cell = score_lookup_table[board];
  if (cell.score > -2) {
    return cell.score;
  }

  int denominator = 0;

  int8_t decoded[9];
  for (int i = 0; i < 9; i++) {
    decoded[i] = (board >> (i*2)) & 3;
    if (decoded[i] == 0) {
      denominator += 1;
    }
  }

  if (isDone(decoded, 1)) {
    cell.score = 1;
  } else if(isDone(decoded, 2)) {
    cell.score = -1;
  } else if(isFull(decoded)) {
    cell.score = 0;
  } else {
    cell.score = 0;
    double reciprocal = 1.0 / double(denominator);
    for (int i = 0; i < 9; i++) {
      for (int p = 1; p < 4; p++) {
        if (decoded[i] != 0) {
          continue;
        }
        int code = board | (p << (i * 2));
        auto child = ComputeRecursive(code);
        cell.score += child * reciprocal * probs[p];
      }
    }
  }
  return cell.score;
}

void InitScoreTable() {
  ComputeRecursive(0);
}
