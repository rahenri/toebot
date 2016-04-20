#ifndef BOARD_H
#define BOARD_H

#include <string>
#include <cassert>

#include "util.h"

using namespace std;

inline bool isDone(const int8_t* cells, int8_t player) {
  return
    (((cells[0]==player) && (cells[1]==player) && (cells[2]==player))) ||
    (((cells[3]==player) && (cells[4]==player) && (cells[5]==player))) ||
    (((cells[6]==player) && (cells[7]==player) && (cells[8]==player))) ||
    (((cells[0]==player) && (cells[3]==player) && (cells[6]==player))) ||
    (((cells[1]==player) && (cells[4]==player) && (cells[7]==player))) ||
    (((cells[2]==player) && (cells[5]==player) && (cells[8]==player))) ||
    (((cells[0]==player) && (cells[4]==player) && (cells[8]==player))) ||
    (((cells[2]==player) && (cells[4]==player) && (cells[6]==player)));
}

class Board {
 public:

  Board() {
    for (int i = 0; i < 9; i++) {
      macrocells[i] = 0;
    }
    for (int i = 0; i < 9*9; i++) {
      cells[i] = 0;
    }
  }

  inline bool wouldBeDone(int cell, int player) const {
    int mcell = cell / 9;
    const int8_t* board = this->cells + (mcell*9);
    int8_t copy[9];
    for (int i = 0; i < 9; i++) {
      copy[i] = board[i];
    }
    copy[cell%9] = player;
    return isDone(copy, player);
  }

  bool canTick(int cell) const {
    int mcell = cell/9;
    return (next_macro == mcell || (next_macro == 9 && macrocells[mcell] == 0)) && (cells[cell] == 0);
  }

  bool isOver() const {
    return isDone(macrocells, 1) || isDone(macrocells, 2);
  }

  bool isDrawn() const {
    for (int i = 0; i < 9*9; i++) {
      if (canTick(i)) {
        return false;
      }
    }
    return true;
  }

  int tick(int cell, int player) {
    cells[cell] = player;

    int ret = next_macro;

    // Check if current macrocell is now taken.
    int mcell = cell/9;
    if (isDone(cells + (mcell*9), player)) {
      macrocells[mcell] = player;
    }

    // Update next macro cell if not taken. It already taken, every not yet
    // taken macrocell is eligible for the next move.
    mcell = cell % 9;
    if (macrocells[mcell] == 0) {
      next_macro = mcell;
    } else {
      next_macro = -1;
      for (int i = 0; i < 9; i++) {
        if (macrocells[i] == 0) {
          if (next_macro == -1) {
            next_macro = i;
          } else {
            next_macro = 9;
            break;
          }
        }
      }
      if (next_macro == -1) {
        next_macro = 9;
      }
    }

    return ret;
  }

  void untick(int cell, int tick_info) {
    int mcell = cell/9;
    cells[cell] = 0;
    macrocells[mcell] = 0;
    next_macro = tick_info;
  }

  bool ParseBoard(const string& repr);
  bool ParseMacroBoard(const string& repr);

  string BoardRepr() const;
  string MacroBoardRepr() const;

  int8_t Cell(int cell) const {
    return this->cells[cell];
  }

  int8_t MacroCell(int cell) const {
    return this->macrocells[cell];
  }

  bool operator==(const Board& other) const;
  bool operator!=(const Board& other) const {
    return !(*this == other);
  }

  int8_t NextMacro() const {
    return next_macro;
  }

 private:

  int8_t cells[9*9];
  int8_t macrocells[9];
  int8_t next_macro = 9;
};

ostream& operator<<(ostream& stream, const Board& board);


#endif
