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
  {0, 1, 2},
  {3, 4, 5},
  {6, 7, 8},
  {0, 3, 6},
  {1, 4, 7},
  {2, 5, 8},
  {0, 4, 8},
  {2, 4, 6},
};

static const float cell_score [9*9] = {
  // +41
  //  0.75258430, 0.71865556,-0.48625118, 0.70279532,
  // -0.30602532,-0.03834203,-0.76526373, 0.04045685,
  //  0.12602348,-0.20369955,-0.69703013,-0.83783892,
  // -0.36743418,-0.87092533,-0.12345948,-0.31132035,
  // -0.13242059, 0.49235975,-0.01657033, 0.24705828,
  //  0.08749625,-0.28738834, 0.51321299, 0.01453005,
  // -0.23329236, 0.32136043, 0.59064964, 0.45136191,
  // -0.29984328, 0.06645361,-0.13619749,-0.05529810,
  // -0.11382633, 0.08938200,-0.16075615, 0.83833204,
  // -0.53967223, 0.02025088, 0.16933006,-0.03338589,
  //  0.13096868, 0.19854530, 0.30511823, 0.14292927,
  // -0.22101272,-0.36262460, 0.05761797,-0.10612649,
  //  0.25920307, 0.16989934, 0.26771048, 0.94792984,
  //  0.32709618, 0.38761059,-0.37469596, 0.71537409,
  // -0.40681474,-0.42834780, 0.32504883, 0.22060762,
  // -0.73482942,-0.71341438, 0.46436534, 0.19170091,
  // -0.13212128, 0.77568484,-0.09737581, 0.08627749,
  //  0.00015503,-0.14226879,-0.09234413, 0.13978162,
  // -0.03911623, 0.41397220, 0.65280305, 0.73456486,
  // -0.69190260, 0.18417232,-0.25598289, 0.03150678,
  //  0.31412925,

  //  Base(./toebot-vanilla):43 Test(./toebot):160 Draws:315 Total:518 Score:61.3±2.5% Rating:+80
  // -0.06009773, 0.04590983, 0.08071016, 0.54504213,
  // -0.11697085,-0.44529016,-0.43405743, 0.07968344,
  // -0.13464969, 0.15888520,-0.37302488,-0.47722104,
  //  0.13109669,-1.21657433, 0.36752716,-0.07675996,
  //  0.12269584,-0.02869565,-0.41321410, 0.15195615,
  // -0.21381283, 0.28616992, 0.65869503, 0.61095806,
  //  0.16580725,-0.31504878, 0.02358666, 0.55089552,
  // -0.25484288, 0.14116337, 0.06181424, 0.13284907,
  // -0.80632959, 0.25534612,-0.32287368, 0.56880082,
  // -0.03051008, 0.25421832,-0.20765697, 0.49914270,
  //  0.48096409,-0.04348376,-0.68907138,-0.29041270,
  // -0.06721895,-0.02885553, 0.04400126,-0.28121437,
  //  0.02201525, 0.49475255, 0.05323116, 0.51537169,
  //  0.41581795, 0.49729562, 0.00829402, 0.19328526,
  // -0.70997804,-0.17104109,-0.09636432, 0.33991152,
  // -0.54886409,-0.48041494, 0.62429715, 0.35295210,
  //  0.35305288, 0.42503014,-0.02743610,-0.33173102,
  //  0.05220332,-0.32612596, 0.16178389, 0.26038349,
  // -0.05637740,-0.51758401, 0.24692510, 0.65143694,
  // -0.25672305,-0.06708090, 0.07893625, 0.11491753,
  //  0.01470733,

  // Base(./toebot-vanilla):31 Test(./toebot):231 Draws:223 Total:485 Score:70.6±2.7% Rating:+152
  // -0.09866529,-0.09491203,-0.03831214, 0.60856920,
  // -0.04650917,-0.51596337,-0.51903748,-0.05097493,
  // -0.28592904, 0.20618234,-0.36092991,-0.25935249,
  //  0.26183658,-1.15259841, 0.24763546,-0.39093804,
  //  0.30316555,-0.17137184,-0.45664104, 0.29515366,
  // -0.03202164, 0.26499037, 0.71479939, 0.67133446,
  //  0.06957090,-0.50637776, 0.39902383, 0.27656760,
  // -0.37003832, 0.25326701, 0.06591885, 0.37180026,
  // -0.57386269, 0.37545480,-0.12232728, 0.31484662,
  //  0.16983872, 0.30875576,-0.18867530, 0.64427186,
  //  0.71298634, 0.13783460,-0.50109761,-0.29568846,
  //  0.19279368,-0.03896737,-0.18372185,-0.22215662,
  // -0.18046174, 0.75290883,-0.06417470, 0.39609597,
  //  0.32720509, 0.62633176,-0.21631491, 0.18821511,
  // -0.51887011,-0.18002945,-0.39131849, 0.30315663,
  // -0.47873371,-0.58034727, 0.39725477, 0.39084953,
  //  0.37611315, 0.04936333, 0.21028300,-0.15089069,
  //  0.02915919,-0.60016550,-0.00044862, 0.20496467,
  // -0.00281376,-0.48811935, 0.29369305, 0.48558962,
  // -0.22970745,-0.01330719,-0.10483609, 0.00725125,
  //  0.06933130,


    -0.24206823, 0.04274534, 0.16690643, 0.36813760,
    -0.12296224,-0.17154056,-0.61208993,-0.21467209,
    -0.04911865, 0.26330156,-0.21272507,-0.41123427,
     0.47357006,-1.07105865, 0.45420354,-0.40220368,
     0.44760024,-0.21087120,-0.19096183, 0.11589532,
    -0.23951059, 0.22847336, 0.71319517, 0.24304872,
    -0.02294357,-0.29178143, 0.50267936, 0.06706877,
     0.12347865, 0.30335093, 0.07348156, 0.07813590,
    -0.26157756, 0.06097422,-0.06374932, 0.00797120,
     0.16493795, 0.01012291,-0.13930986, 0.36948985,
     0.61601398,-0.07793052,-0.50241534,-0.35067709,
     0.01024236, 0.03690561,-0.05143001,-0.32425733,
    -0.12876465, 0.37955498, 0.18698933, 0.20559287,
     0.41819607, 0.55253863,-0.12398123, 0.29908725,
    -0.45399272, 0.04462166,-0.54933492, 0.29630938,
    -0.68813264,-0.61502800, 0.18047864, 0.18453899,
     0.42715648, 0.20539324, 0.10925075,-0.12312346,
     0.31236163,-0.61413204,-0.18383971, 0.28130118,
     0.12908390,-0.58028979, 0.07717310, 0.17376403,
     0.14509191, 0.20768449, 0.08215218, 0.03682770,
    -0.06044208,

  };

static const double cell_bias = 0.0;

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
    double sum = cell_bias;
    for (int i = 0; i < 9*9; i++) {
      double v = (cells[i] == 1 ? -1 : (cells[i] == 2 ? 1 : 0));
      sum += v * cell_score[i];
    }
    sum *= 0.01;

    for (const auto& line : WinLines) {
      sum += (micro_win_prob[boards_code[line[0]]] * micro_win_prob[boards_code[line[1]]] * micro_win_prob[boards_code[line[2]]])
        - (micro_lose_prob[boards_code[line[0]]] * micro_lose_prob[boards_code[line[1]]] * micro_lose_prob[boards_code[line[2]]]);
    }
    int score = sum * 100000;
    return (player == 1) ? score : -score;
  }

  int ply() const;

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
