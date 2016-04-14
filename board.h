#ifndef BOARD_H
#define BOARD_H

#include <string>

using namespace std;

struct Board {
  int8_t cells[9*9];
  int8_t macrocells[9];

  bool canTick(int cell) {
    int mcell = cell / 9;
    if (macrocells[mcell] != -1) {
      return false;
    }
    return cells[cell] == 0;
  }

  bool isOver() {
    return isDone(macrocells, 1) || isDone(macrocells, 2);
  }

  bool isDone(int8_t* cells, int8_t player) {
    return
      ((cells[0]==player && cells[1]==player && cells[2]==player)) ||
      ((cells[3]==player && cells[4]==player && cells[5]==player)) ||
      ((cells[6]==player && cells[7]==player && cells[8]==player)) ||
      ((cells[0]==player && cells[3]==player && cells[6]==player)) ||
      ((cells[1]==player && cells[4]==player && cells[7]==player)) ||
      ((cells[2]==player && cells[5]==player && cells[8]==player)) ||
      ((cells[0]==player && cells[4]==player && cells[8]==player)) ||
      ((cells[2]==player && cells[4]==player && cells[6]==player));
  }

  void tick(int cell, int player) {
    cells[cell] = player;

    // Check if current macrocell is now taken.
    int mcell = cell/9;
    if (isDone(cells + (mcell*9), player)) {
      macrocells[mcell] = player;
    }

    // Update next macrocell

    // Which macrocell should be next
    mcell = cell % 9;

    // Clear out allowed macrocell.
    for (int i = 0; i < 9; i++) {
      if (macrocells[i] == -1) {
        macrocells[i] = 0;
      }
    }

    // Update next macro cell if not taken. It already taken, every not yet
    // taken macrocell is eligible for the next move.
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
};

void parseBoard(Board* board, const string& repr);
void parseMacroBoard(Board* board, const string& repr);

#endif
