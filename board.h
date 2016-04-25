#ifndef BOARD_H
#define BOARD_H

#include <string>
#include <cassert>
#include <cstdint>

#include "util.h"
#include "score_table.h"
#include "hash.h"

using namespace std;

void InitBoardConstants();

// Constants
extern uint16_t captureMoveIndex[1<<18][2];
extern int8_t captureMoveLookup[33270];

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

inline void DecodeBoard(int8_t* board, uint32_t code) {
  for (int i = 0; i < 9; i++) {
    board[i] = (code >> (2 * i)) & 3;
  }
}

class Board {
 public:

  Board() {
    for (int i = 0; i < 9; i++) {
      macrocells[i] = 0;
      boards_code[i] = 0;
    }
    for (int i = 0; i < 9*9; i++) {
      cells[i] = 0;
    }
    hash = HashBoard(this);
  }

  inline bool wouldBeDone(int cell, int player) const {
    int mcell = cell / 9;
    const int8_t* board = this->cells + (mcell*9);
    return isDoneWithCell(board, cell, player);
  }

  inline bool canTick(int cell) const {
    int mcell = cell/9;
    return (next_macro == mcell || (next_macro == 9 && macrocells[mcell] == 0)) && (cells[cell] == 0);
  }

  inline bool isOver() const {
    return done;
  }

  inline bool IsDrawn() const {
    return draw;
  }

  int tick(int cell, int player) {
    int mcell = cell/9;
    int bcell = cell%9;

    cells[cell] = player;
    boards_code[mcell] ^= player << (bcell * 2);

    int ret = next_macro;

    // Check if current macrocell is now taken.
    const int8_t* b = cells + (mcell*9);
    if (isDoneWithCell(b, bcell, player)) {
      macrocells[mcell] = player;
      macroboard_code |= player << (mcell * 2);
      if (isDoneWithCell(macrocells, mcell, player)) {
        done = true;
      }
    }  else if(isFull(b)) {
      macrocells[mcell] = 3;
      macroboard_code |= 3 << (mcell * 2);
    }

    // Update next macro cell if not taken. It is already taken, every not yet
    // taken macrocell is eligible for the next move.
    mcell = bcell;
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
        draw = true;
      }
    }

    hash = UpdateHash(hash, next_macro, ret, cell, player);

    return ret;
  }

  void untick(int cell, int tick_info) {
    int mcell = cell/9;
    int bcell = cell%9;
    int player = cells[cell];
    cells[cell] = 0;
    boards_code[mcell] ^= player << (bcell * 2);
    macroboard_code ^= macrocells[mcell] << (mcell * 2);
    macrocells[mcell] = 0;
    hash = UpdateHash(hash, next_macro, tick_info, cell, player);
    next_macro = tick_info;
    done = false;
    draw = false;
  }

  inline int ListCaptureMoves(uint8_t* moves, int player) {
    int move_count = 0;
    if (next_macro != 9) {
      auto offset = captureMoveIndex[boards_code[next_macro]][player-1];
      if (offset != 0xffff) {
        int k = next_macro * 9;
        while (captureMoveLookup[offset]>=0) {
          moves[move_count++] = captureMoveLookup[offset++] + k;
        }
      }
    } else {
      for (int mcell = 0; mcell < 9; mcell++) {
        if (macrocells[mcell] != 0) {
          continue;
        }
        auto offset = captureMoveIndex[boards_code[mcell]][player-1];
        if (offset != 0xffff) {
          int k = mcell * 9;
          while (captureMoveLookup[offset]>=0) {
            moves[move_count++] = captureMoveLookup[offset++] + k;
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

  inline int Eval(int player) {
    int score = int(score_lookup_table[macroboard_code].score * 10000);
    return (player == 1) ? score : -score;
  }

 private:

  void RegenState();

  int8_t cells[9*9];
  int8_t macrocells[9];
  int8_t next_macro = 9;
  uint64_t hash;

  uint32_t boards_code[9];

  uint32_t macroboard_code = 0;

  bool done = false;
  bool draw = false;
};

ostream& operator<<(ostream& stream, const Board& board);


#endif
