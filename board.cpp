#include <iostream>
#include <string>

#include "board.h"
#include "util.h"

using namespace std;

uint16_t captureMoveIndex[1<<18][2];
int8_t captureMoveLookup[33270];

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
  RegenState();
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
  if (next_macro == -1) {
    next_macro = 9;
  }
  // We don't update macroboard here, we can compute the rest from the board
  // itself.
  RegenState();
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

void Board::RegenState() {
  macroboard_code = 0;
  for (int i = 0; i < 9; ++i) {
    const int8_t* b = cells + (i*9);
    if (isDone(b, 1)) {
      macrocells[i] = 1;
    } else if (isDone(b, 2)) {
      macrocells[i] = 2;
    } else if (isFull(b))  {
      macrocells[i] = 3;
    } else {
      macrocells[i] = 0;
    }

    macroboard_code |= macrocells[i] << (i * 2);
    
    boards_code[i] = 0;
    for (int j = 0; j < 9; j++) {
      boards_code[i] |= cells[j + i*9] << (j * 2);
    }
  }

  reg_score_inc = reg_cell_bias_int;
  for (int i = 0; i < 9*9; i++) {
    if (cells[i] == 1) {
      reg_score_inc -= reg_cell_score_int[i];
    } else if(cells[i] == 2) {
      reg_score_inc += reg_cell_score_int[i];
    }
  }

  hash = HashBoard(this);
  this->done = (isDone(this->macrocells, 1) || isDone(macrocells, 2));
  this->draw = isFull(this->macrocells);
}

void InitCaptureMoves() {
  int offset = 0;
  for (int code = 0; code < (1 << 18); code++) {
    int8_t board[9];
    DecodeBoard(board, code);
    if (isDone(board, 1) || isDone(board, 2) || isFull(board)) {
      continue;
    }
    bool bad = false;
    for (int cell = 0; cell < 9; cell++) {
      if (board[cell] == 3) {
        bad = true;
      }
    }
    if (bad) {
      continue;
    }
    for (int player = 1; player <= 2; player++) {
      int8_t moves[9];
      int count = 0;
      for (int cell = 0; cell < 9; cell++) {
        if (board[cell] != 0) {
          continue;
        }
        if (isDoneWithCell(board, cell, player)) {
          moves[count++] = cell;
        }
      }
      if (count == 0) {
        captureMoveIndex[code][player-1] = 0xffff;
        continue;
      }
      captureMoveIndex[code][player-1] = offset;
      for (int j = 0; j < count; j++) {
        captureMoveLookup[offset++] = moves[j];
      }
      captureMoveLookup[offset++] = -1;
    }
  }
  cerr << "Capture move lookup size: " << offset << endl;
}

void InitBoardConstants() {
  InitCaptureMoves();
}

int Board::ply() const {
  int ply = 1;
  for (int i = 0; i < 9*9; i++) {
    if (this->Cell(i) != 0) {
      ply ++;
    }
  }
  return ply;
}
