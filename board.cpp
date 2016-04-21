#include <iostream>
#include <string>

#include "board.h"
#include "util.h"

using namespace std;

bool Board::ParseBoard(const string& repr) {
  auto parts = parseCSV(repr);
  if (parts.size() != 9*9) {
    cerr << "Bad board repr size:" << parts.size() << endl;
    return false;
  }
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      int k = i*9+j;
      this->cells[encodeCell(i, j)] = stoi(parts[k]);
    }
  }
  RecomputeMacroBoard();
  hash = HashBoard(this);
  this->done = (isDone(this->macrocells, 1) || isDone(macrocells, 2));
  return true;
}

bool Board::ParseMacroBoard(const string& repr) {
  auto parts = parseCSV(repr);
  if (parts.size() != 9) {
    cerr << "Bad macro board size: " << parts.size() << endl;
    return false;
  }
  next_macro = -1;
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      int k = i*3+j;
      int v = stoi(parts[k]);
      if (v == -1) {
        v = 0;
        if (next_macro == -1) {
          next_macro = k;
        } else {
          next_macro = 9;
        }
      }
    }
  }
  // We don't update macroboard here, we can compute the rest from the board
  // itself.
  hash = HashBoard(this);
  return true;
}

ostream& operator<<(ostream& stream, const Board& board) {
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      int cell = encodeCell(i, j);
      int value = board.Cell(cell);
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
      out += to_string(int(Cell(cell)));
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
    int v = MacroCell(i);
    if (v == 0 && (next_macro == 9 || next_macro == i)) {
      v = -1;
    }
    if (v == 3) {
      v = 0;
    }
    out += to_string(v);
  }
  return out;
}

bool Board::operator==(const Board& other) const {
  if (next_macro != other.next_macro) {
    return false;
  }
  for (int i = 0; i < 9*9; i++) {
    if (cells[i] != other.cells[i]) {
      return false;
    }
  }
  for (int i = 0; i < 9; i++) {
    if (macrocells[i] != other.macrocells[i]) {
      return false;
    }
  }
  return true;
}
