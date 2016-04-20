#include <random>
#include <iostream>

#include "hash.h"
#include "board.h"

using namespace std;

uint64_t cell_masks[9*9][3];
uint64_t next_masks[10];

void InitHashConstants() {
  mt19937_64 generator;
  generator.seed(0x438ead462650e4a);
  for (int i = 0; i < 9*9; i++) {
    for (int j = 0; j < 3; j++) {
      cell_masks[i][j] = generator();
    }
  }
  for (int i = 0; i < 10; i++) {
    next_masks[i] = generator();
  }
}

uint64_t HashBoard(const Board* board) {
  assert(board->NextMacro() >= 0 && board->NextMacro() < 10);
  uint64_t out = next_masks[board->NextMacro()];
  for (int i = 0; i < 9*9; i++) {
    out ^= cell_masks[i][board->Cell(i)];
  }
  if (out == 0) {
    cerr << "WARNING: hash is 0" << endl;
  }
  return out;
}
