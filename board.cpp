#include <iostream>

#include "board.h"
#include "util.h"

void parseBoard(Board* board, const string& repr) {
  if (repr.size() != 81*2-1) {
    cerr << "Bad board repr size:" << repr.size() << endl;
    return;
  }
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      int k = i*9+j;
      board->cells[encodeCell(i, j)] = repr[k*2] - '0';
    }
  }
}

void parseMacroBoard(Board* board, const string& repr) {
  auto parts = parseCSV(repr);
  if (parts.size() != 9) {
    cerr << "Bad macro board size: " << parts.size() << endl;
    return;
  }
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      int k = i*3+j;
      board->macrocells[k] = stoi(parts[k]);
    }
  }
}

