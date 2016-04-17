#include <algorithm>
#include <chrono>
#include <iostream>

#include "ai.h"
#include "random.h"
#include "hash.h"

using namespace std::chrono;

static const int PlayDeterministic = false;

static const int DrawPenalty = 50;
static const int BoardValue = 1000;
static const int MaxDepth = PlayDeterministic ? 10 : 50;

int leafEval(const Board *board, int player) {
  int out = 0;
  int other = 3-player;
  for (int i = 0; i < 9; i++) {
    auto c = board->MacroCell(i);
    if (c == player) {
      out += BoardValue;
    } else if (c == other) {
      out -= BoardValue;
    }
  }
  return out;
}


struct TimeLimitExceeded {
};

struct MoveInfo {
  int8_t score;
  int8_t move;
  bool operator<(const MoveInfo& other) const {
    return this->move > other.move;
  }
};

void sortMoves(const Board* board, int8_t *moves, int count, int player) {
  MoveInfo move_infos[9*9];
  for (int i = 0; i < count; i++) {
    int8_t cell = moves[i];
    move_infos[i].move = cell;
    bool done = board->wouldBeDone(cell, player);
    move_infos[i].score = done ? 1 : 0;
  }
  stable_sort(move_infos, move_infos+count);
  for (int i = 0; i < count; i++) {
    moves[i] = move_infos[i].move;
  }
}

// Sorting seems to make things faster on average, but it makes it
// occasionally much slower. Leave it disabled for now until I can avoid
// that.
int listMoves(const Board* board, int8_t* moves, int player, bool need_sorting) {
  int count = 0;
  for (int cell = 0; cell < 9*9; cell++) {
    if (!board->canTick(cell)) {
      continue;
    }
    moves[count++] = cell;
  }
  if (need_sorting) {
    sortMoves(board, moves, count, player);
  }
  return count;
}


struct AI {
  int nodes = 0;
  int time_limit = 0;
  int deadline_counter = 0;
  int initial_player = 0;
  steady_clock::time_point deadline;
  bool has_deadline;
  HashTable* table;

  AI(HashTable* table, int time_limit) : table(table) {
    if (time_limit == 0 || PlayDeterministic) {
      this->has_deadline = false;
    } else {
      auto now = steady_clock::now();
      this->deadline = now + milliseconds(time_limit);
      this->has_deadline = true;
    }
  }

  void checkDeadline() {
    if (!this->has_deadline) {
      return;
    }
    this->deadline_counter++;
    if (this->deadline_counter % (1<<14) != 0) {
      return;
    }
    auto now = steady_clock::now();
    if (now >= this->deadline) {
      throw TimeLimitExceeded();
    }
  }

  int DeepEval(const Board *board, int player, int ply, int depth, int alpha, int beta) {
    int best_score = -MaxScore;
    this->nodes++;
    this->checkDeadline();

    if (board->isOver()) {
      return -MaxScore + ply;
    }

    if (board->isDrawn()) {
      return DrawPenalty;
    }

    if (depth == 0) {
      return leafEval(board, player);
    }

    {
      int upper_bound = MaxScore - (ply + 1);
      if (upper_bound < alpha) {
        return upper_bound;
      }
    }

    int first_cell = 0;
    auto memo = this->table->Get(board);
    if (memo != nullptr) {
      if (memo->depth >= depth) {
        if (memo->lower_bound == memo->upper_bound) {
          return memo->lower_bound;
        }
        if (memo->lower_bound > beta) {
          return memo->lower_bound;
        }
        if (memo->upper_bound < alpha) {
          return memo->upper_bound;
        }
      }
      first_cell = memo->move;
    }

    int best_move = -1;
    for (int i = 0; i < 9*9; i++) {
      int cell = (i == 0) ? first_cell : ((i == first_cell) ? 0 : i);

      if (!board->canTick(cell)) {
        continue;
      }
      Board copy = *board;
      copy.tick(cell, player);
      int score = -this->DeepEval(&copy, 3-player, ply+1, depth-1, -beta, -alpha);
      if (score > best_score) {
        best_score = score;
        alpha = max(alpha, score+1);
        best_move = cell;
        if (score > beta) {
          break;
        }
      }
    }
    int lower_bound = -MaxScore;
    int upper_bound = MaxScore;
    if (best_score > beta) {
      lower_bound = beta+1;
    } else if (best_score < alpha) {
      upper_bound = alpha-1;
    } else {
      lower_bound = upper_bound = best_score;
    }
    this->table->Insert(board, lower_bound, upper_bound, depth, best_move);
    return best_score;
  }

  SearchResult SearchMove(const Board *board, int player, int ply, int depth) {
    this->initial_player = player;
    SearchResult out;
    out.score = -MaxScore;
    out.depth = depth;
    out.move = -1;

    this->nodes++;

    int alternatives[9*9];
    int alternative_count = 0;

    int first_cell = 0;
    auto memo = this->table->Get(board);
    if (memo != nullptr) {
      first_cell = memo->move;
    }

    for (int i = 0; i < 9*9; i++) {
      int cell = (i == 0) ? first_cell : ((i == first_cell) ? 0 : i);
      if (!board->canTick(cell)) {
        continue;
      }
      Board copy = *board;
      copy.tick(cell, player);
      int score = -this->DeepEval(&copy, 3-player, ply+1, depth-1, -MaxScore, -out.score);
      if (score > out.score || out.move == -1) {
        out.score = score;
        out.move = cell;
        alternatives[0] = cell;
        alternative_count = 1;
      } else if (score == out.score) {
        alternatives[alternative_count] = cell;
        alternative_count++;
      }
    }
    this->table->Insert(board, out.score, out.score, depth, out.move);

    if (PlayDeterministic) {
      if (alternative_count > 0) {
        out.move = *min_element(alternatives, alternatives+alternative_count);
      }
    } else {
      if (alternative_count > 1) {
        out.move = alternatives[RandN(alternative_count)];
      }
    }

    out.nodes = nodes;

    return out;
  }
};

SearchResult SearchMove(HashTable* table, const Board *board, int player, int time_limit) {
  int ply = 1;
  for (int i = 0; i < 9*9; i++) {
    if (board->Cell(i) != 0) {
      ply ++;
    }
  }

  AI ai(table, time_limit);
  SearchResult out;
  for (int depth = 2; depth <= MaxDepth; depth += 2) {
    SearchResult tmp;
    try {
      tmp = ai.SearchMove(board, player, ply, depth);
    } catch (TimeLimitExceeded e) {
      cerr << "Search interrupted after reaching time limit of " << time_limit << " milliseconds" << endl;
      break;
    }
    out = tmp;
  }
  return out;
}
