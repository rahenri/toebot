#ifndef BOARD_H
#define BOARD_H

#include <string>
#include <cassert>
#include <cstdint>

#include "util.h"
#include "hash.h"

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

inline bool isDoneWithCell(const int8_t* cells, int cell, int8_t player) {
  switch(cell) {
    case 0:
      return
        (cells[1]==player && cells[2]==player) ||
        (cells[3]==player && cells[6]==player) ||
        (cells[4]==player && cells[8]==player);
    case 1:
      return
        (cells[0]==player && cells[2]==player) ||
        (cells[4]==player && cells[7]==player);
    case 2:
      return
        (cells[0]==player && cells[1]==player) ||
        (cells[5]==player && cells[8]==player) ||
        (cells[4]==player && cells[6]==player);
    case 3:
      return
        (cells[4]==player && cells[5]==player) ||
        (cells[0]==player && cells[6]==player);
    case 4:
      return
        (cells[3]==player && cells[5]==player) ||
        (cells[1]==player && cells[7]==player) ||
        (cells[0]==player && cells[8]==player) ||
        (cells[2]==player && cells[6]==player);
    case 5:
      return
        (cells[3]==player && cells[4]==player) ||
        (cells[2]==player && cells[8]==player);
    case 6:
      return
        (cells[7]==player && cells[8]==player) ||
        (cells[0]==player && cells[3]==player) ||
        (cells[2]==player && cells[4]==player);
    case 7:
      return
        (cells[6]==player && cells[8]==player) ||
        (cells[1]==player && cells[4]==player);
    case 8:
      return
        (cells[6]==player && cells[7]==player) ||
        (cells[2]==player && cells[5]==player) ||
        (cells[0]==player && cells[4]==player);
  }
  assert(false);
  return false;
}

inline bool isFull(const int8_t* cells) {
  return
    (cells[0]!=0) &&
    (cells[1]!=0) &&
    (cells[2]!=0) &&
    (cells[3]!=0) &&
    (cells[4]!=0) &&
    (cells[5]!=0) &&
    (cells[6]!=0) &&
    (cells[7]!=0) &&
    (cells[8]!=0);
}

class Board {
 public:

  Board() {
    for (int i = 0; i < 9; i++) {
      macrocells[i] = 0;
      boards[i] = 0;
    }
    for (int i = 0; i < 9*9; i++) {
      cells[i] = 0;
    }
    hash = HashBoard(this);
  }

  inline bool wouldBeDone(int cell, int player) const {
    int mcell = cell / 9;
    const int8_t* board = this->cells + (mcell*9);
    int8_t copy[9];
    for (int i = 0; i < 9; i++) {
      copy[i] = board[i];
    }
    copy[cell%9] = player;
    return isDoneWithCell(copy, cell, player);
  }

  bool canTick(int cell) const {
    int mcell = cell/9;
    return (next_macro == mcell || (next_macro == 9 && macrocells[mcell] == 0)) && (cells[cell] == 0);
  }

  bool isOver() const {
    return done;
  }

  bool IsDrawn() const {
    for (int i = 0; i < 9; i++) {
      if (macrocells[i] == 0) {
        return false;
      }
    }
    return true;
  }

  void RecomputeMacroBoard() {
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
    }
  }

  int tick(int cell, int player) {
    cells[cell] = player;

    int ret = next_macro;

    // Check if current macrocell is now taken.
    int mcell = cell/9;
    const int8_t* b = cells + (mcell*9);
    if (isDoneWithCell(b, cell%9, player)) {
      macrocells[mcell] = player;
      if (isDoneWithCell(macrocells, mcell, player)) {
        done = true;
      }
    }  else if(isFull(b)) {
      macrocells[mcell] = 3;
    }

    // Update next macro cell if not taken. It is already taken, every not yet
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

    hash = UpdateHash(hash, next_macro, ret, cell, player);

    return ret;
  }

  void untick(int cell, int tick_info) {
    int mcell = cell/9;
    int player = cells[cell];
    cells[cell] = 0;
    macrocells[mcell] = 0;
    hash = UpdateHash(hash, next_macro, tick_info, cell, player);
    next_macro = tick_info;
    done = false;
  }

  inline int ListCaptureMoves(uint8_t* moves, int player) {
    int move_count = 0;
    if (next_macro != 9) {
      int offset = next_macro*9;
      for (int cell = offset; cell < offset+9; cell++) {
        if (cells[cell] != 0) {
          continue;
        }
        if (isDoneWithCell(cells+offset, cell-offset, player)) {
          moves[move_count++] = cell;
        }
      }
    } else {
      for (int mcell = 0; mcell < 9; mcell++) {
        if (macrocells[mcell] != 0) {
          continue;
        }
        int offset = mcell*9;
        for (int cell = offset; cell < offset+9; cell++) {
          if (cells[cell] != 0) {
            continue;
          }
          if (isDoneWithCell(cells+offset, cell-offset, player)) {
            moves[move_count++] = cell;
          }
        }
      }
    }
    return move_count;
  }

  inline int ListMoves(uint8_t* moves, int first_move) {
    int move_count = 0;
    if (first_move != -1) {
      moves[move_count++] = first_move;
    }
    if (next_macro != 9) {
      int offset = next_macro*9;
      for (int cell = offset; cell < offset+9; cell++) {
        if (first_move == cell || (cells[cell] != 0)) {
          continue;
        }
        moves[move_count++] = cell;
      }
    } else {
      for (int mcell = 0; mcell < 9; mcell++) {
        if (macrocells[mcell] != 0) {
          continue;
        }
        int offset = mcell*9;
        for (int cell = offset; cell < offset+9; cell++) {
          if (first_move == cell || (cells[cell] != 0)) {
            continue;
          }
          moves[move_count++] = cell;
        }
      }
    }
    return move_count;
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

  uint64_t Hash() const {
    return hash;
  }

 private:

  int8_t cells[9*9];
  int8_t macrocells[9];
  int8_t next_macro = 9;
  uint64_t hash;
  uint32_t boards[9];
  bool done = false;
};

ostream& operator<<(ostream& stream, const Board& board);


#endif
