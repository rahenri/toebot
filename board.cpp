#include <iostream>
#include <string>

#include "board.h"
#include "util.h"

using namespace std;

void parseBoard(Board* board, const string& repr) {
  auto parts = parseCSV(repr);
  if (parts.size() != 9*9) {
    cerr << "Bad board repr size:" << parts.size() << endl;
    return;
  }
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      int k = i*9+j;
      board->cells[encodeCell(i, j)] = stoi(parts[k]);
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

ostream& operator<<(ostream& stream, const Board& board) {
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      int cell = encodeCell(i, j);
      int value = board.cells[cell];
      if (value == 1) {
        stream << "x";
      } else if (value == 2) {
        stream << "o";
      } else {
        stream << ".";
      }
    }
    stream << endl;
  }
  return stream;
}

string Board::BoardRepr() const {
  string out;
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      if (out.size() > 0) {
        out += ',';
      }
      int cell = encodeCell(i, j);
      out += to_string(int(cells[cell]));
    }
  }
  return out;
}

string Board::MacroBoardRepr() const {
  string out;
  for (int i = 0; i < 9; i++) {
    if (out.size() > 0) {
      out += ',';
    }
    out += to_string(macrocells[i]);
  }
  return out;
}
