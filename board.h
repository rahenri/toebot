#ifndef BOARD_H
#define BOARD_H

#include <string>

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

struct Board {
  int8_t cells[9*9];
  int8_t macrocells[9];

  Board() {
    for (int i = 0; i < 9; i++) {
      macrocells[i] = -1;
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
    return (macrocells[cell/9] == -1) && (cells[cell] == 0);
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

  void tick(int cell, int player) {
    cells[cell] = player;

    // Check if current macrocell is now taken.
    int mcell = cell/9;
    if (isDone(cells + (mcell*9), player)) {
      macrocells[mcell] = player;
    }

    // Update allowed macrocell

    // Clear out allowed macrocell.
    for (int i = 0; i < 9; i++) {
      if (macrocells[i] == -1) {
        macrocells[i] = 0;
      }
    }

    // Update next macro cell if not taken. It already taken, every not yet
    // taken macrocell is eligible for the next move.
    mcell = cell % 9;
    if (macrocells[mcell] == 0) {
      macrocells[mcell] = -1;
    } else {
      for (int i = 0; i < 9; i++) {
        if (macrocells[i] == 0) {
          macrocells[i] = -1;
        }
      }
    }
  }

  string BoardRepr() const;
  string MacroBoardRepr() const;

};

ostream& operator<<(ostream& stream, const Board& board);

void parseBoard(Board* board, const string& repr);
void parseMacroBoard(Board* board, const string& repr);

#endif
