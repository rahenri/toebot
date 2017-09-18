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

static const int MaxScore = 2000000000;

static const int8_t WinLines[][3] = {
  {0, 1, 2}, // 0
  {3, 4, 5}, // 1
  {6, 7, 8}, // 2
  {0, 3, 6}, // 3
  {1, 4, 7}, // 4
  {2, 5, 8}, // 5
  {0, 4, 8}, // 6
  {2, 4, 6}, // 7
};

static const int8_t LinesByPos[][5] = {
  {0, 3, 6, -1}, // 0
  {0, 4, -1},    // 1
  {0, 5, 7, -1}, // 2
  {1, 3, -1},    // 3
  {1, 4, 6, 7, -1}, // 4
  {1, 5, -1},    // 5
  {2, 3, 7, -1}, // 6
  {2, 4, -1},    // 7
  {2, 5, 6, -1}, // 8
};

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

  int tick(int cell) {
    int mcell = cell/9;
    int bcell = cell%9;

    cells[cell] = turn;
    assert(bcell < 9);
    boards_code[mcell] ^= turn << (bcell * 2);
    assert(boards_code[mcell] < (1<<18));
    if (turn == 1) {
      reg_score_inc -= reg_cell_score_int[cell];
    } else {
      reg_score_inc += reg_cell_score_int[cell];
    }

    int ret = next_macro;

    // Check if current macrocell is now taken.
    const int8_t* b = cells + (mcell*9);
    if (isDoneWithCell(b, bcell, turn)) {
      macrocells[mcell] = turn;
      macroboard_code |= turn << (mcell * 2);
      if (isDoneWithCell(macrocells, mcell, turn)) {
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

    hash = UpdateHash(hash, next_macro, ret, cell, turn);

    turn ^= 3;

    return ret;
  }

  void untick(int cell, int tick_info) {
    turn ^= 3;

    int mcell = cell/9;
    int bcell = cell%9;
    cells[cell] = 0;
    assert(bcell < 9);
    boards_code[mcell] ^= turn << (bcell * 2);
    assert(boards_code[mcell] < (1<<18));
    macroboard_code ^= macrocells[mcell] << (mcell * 2);
    macrocells[mcell] = 0;
    hash = UpdateHash(hash, next_macro, tick_info, cell, turn);
    next_macro = tick_info;
    done = false;
    draw = false;
    if (turn == 1) {
      reg_score_inc += reg_cell_score_int[cell];
    } else {
      reg_score_inc -= reg_cell_score_int[cell];
    }
  }

  inline int ListCaptureMoves(uint8_t* moves) {
    int move_count = 0;
    if (next_macro != 9) {
      auto offset = captureMoveIndex[boards_code[next_macro]][turn-1];
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
        auto offset = captureMoveIndex[boards_code[mcell]][turn-1];
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

  void ExtractField(int8_t* field, int8_t* macroboard) const;

  void PutField(int8_t* field, int8_t* macroboard);


  inline int Eval() {
    int base = int(Heuristic() * score_factor);
    int base2 = int(Heuristic2() * 0.3 * score_factor);
    int score = base + base2 + (reg_score_inc);
    return ((turn == 1) ? score : -score);
  }

  inline double Heuristic() {
    double sum = 0;
    for (int i = 0; i < 8; i++) {
      const auto* line = WinLines[i];
      sum += (micro_win_prob[boards_code[line[0]]] * micro_win_prob[boards_code[line[1]]] * micro_win_prob[boards_code[line[2]]])
        - (micro_lose_prob[boards_code[line[0]]] * micro_lose_prob[boards_code[line[1]]] * micro_lose_prob[boards_code[line[2]]]);

    }
    return sum;
  }

  inline double Heuristic2() {
    if (next_macro == 9) {
      return 0.2;
      double best = (turn == 1) ? -1e9 : 1e9;;
      for (int i = 0; i < 9; i++) {
        if (macrocells[i] != 0) {
          continue;
        }
        double value = Heuristic2p2(i, turn << (2 * (best_cell_table[turn-1][boards_code[i]])));
        if (turn == 1) {
          best = max(best, value);
        } else {
          best = min(best, value);
        }
      }
      return best;
    } else {
      return Heuristic2p2(next_macro, turn << (2 * (best_cell_table[turn-1][boards_code[next_macro]])));
    }
  }

  inline double Heuristic2p2(int macro, int mask) {
    double before = Heuristic2p1(macro);
    assert(mask < (1<<18));
    boards_code[macro] ^= mask;
    double after = Heuristic2p1(macro);
    boards_code[macro] ^= mask;
    return after - before;
  }

  inline double Heuristic2p1(int macro) {
    double sum = 0;
    for (int i = 0; i < 9; i++) {
      assert(boards_code[i] < (1<<18));
    }
    for (int i = 0; LinesByPos[macro][i] != -1; i++) {
      const auto* line = WinLines[LinesByPos[macro][i]];
      sum += (micro_win_prob[boards_code[line[0]]] * micro_win_prob[boards_code[line[1]]] * micro_win_prob[boards_code[line[2]]])
        - (micro_lose_prob[boards_code[line[0]]] * micro_lose_prob[boards_code[line[1]]] * micro_lose_prob[boards_code[line[2]]]);

    }
    return sum;
  }

  int ply() const;
  inline int Turn() const {
    return turn;
  }
  inline void SetTurn(int player) {
    if (player != turn) {
      this->turn = player;
      RegenState();
    }
  }


  void RegenState();

  void Rotate();
  void Mirror();

 private:

  int8_t turn = 1;

  // All cells values are either 0(empty), one (cross), two (circle).
  // Each microboard is consecutive
  int8_t cells[9*9];
  // Macrocells, values are either 0(empty), one(cross won), two (circle won)
  // or three(full without winner).
  int8_t macrocells[9];
  // Next macro cell to play, or 9 if all.
  int8_t next_macro = 9;
  // Current board hash.
  uint64_t hash;

  uint32_t boards_code[9];

  uint32_t macroboard_code = 0;

  bool done = false;
  bool draw = false;

  int reg_score_inc = reg_cell_bias_int;
};

ostream& operator<<(ostream& stream, const Board& board);


#endif
