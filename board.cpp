#include <iostream>
#include <string>

#include "board.h"
#include "util.h"

using namespace std;

void Board::parseBoard(const string& repr) {
  auto parts = parseCSV(repr);
  if (parts.size() != 9*9) {
    cerr << "Bad board repr size:" << parts.size() << endl;
    return;
  }
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      int k = i*9+j;
      this->cells[encodeCell(i, j)] = stoi(parts[k]);
    }
  }
}

void Board::parseMacroBoard(const string& repr) {
  auto parts = parseCSV(repr);
  if (parts.size() != 9) {
    cerr << "Bad macro board size: " << parts.size() << endl;
    return;
  }
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      int k = i*3+j;
      this->macrocells[k] = stoi(parts[k]);
    }
  }
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
    out += to_string(int(MacroCell(i)));
  }
  return out;
}

bool Board::operator==(const Board& other) const {
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
