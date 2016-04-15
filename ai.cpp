#include "ai.h"

#include <algorithm>
#include <cstdlib>
#include <chrono>
#include <iostream>

using namespace std::chrono;

int leafEval(const Board *board, int player) {
  int out = 0;
  int other = 3-player;
  for (auto c : board->macrocells) {
    if (c == player) {
      out ++;
    } else if (c == other) {
      out --;
    }
  }
  return out;
}

struct TimeLimitExceeded {
};

struct AI {
  int nodes = 0;
  int time_limit = 0;
  int deadline_counter = 0;
  steady_clock::time_point deadline;
  bool has_deadline;

  AI(int time_limit) {
    if (time_limit == 0) {
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

  int DeepEval(const Board *board, int player, int depth, int alpha, int beta) {
    int best_score = -MaxScore;
    this->nodes++;
    this->checkDeadline();

    if (board->isOver()) {
      return -MaxScore - depth + 100;
    }

    if (board->isDrawn()) {
      return 0;
    }

    if (depth == 0) {
      return leafEval(board, player);
    }

    for (int cell = 0; cell < 9*9; cell++) {
      if (!board->canTick(cell)) {
        continue;
      }
      Board copy = *board;
      copy.tick(cell, player);
      int score = -this->DeepEval(&copy, 3-player, depth-1, -beta, -alpha);
      if (score > best_score) {
        best_score = score;
        alpha = max(alpha, score+1);
        if (score > beta) {
          break;
        }
      }
    }
    return best_score;
  }

  SearchResult SearchMove(const Board *board, int player, int depth) {
    SearchResult out;
    out.score = -MaxScore;
    out.depth = depth;
    this->nodes++;

    int alternatives[9*9];
    int alternative_count = 0;

    for (int cell = 0; cell < 9*9; cell++) {
      if (!board->canTick(cell)) {
        continue;
      }
      Board copy = *board;
      copy.tick(cell, player);
      int score = -this->DeepEval(&copy, 3-player, depth-1, -MaxScore, -out.score);
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

    if (alternative_count > 1) {
      out.move = alternatives[rand() % alternative_count];
    }

    out.nodes = nodes;

    return out;
  }
};

static const int MaxDepth = 10;

SearchResult SearchMove(const Board *board, int player, int time_limit) {
  AI ai(time_limit);
  SearchResult out;
  try {
    for (int depth = 2; depth <= MaxDepth; depth += 2) {
     SearchResult tmp = ai.SearchMove(board, player, depth);
     out = tmp;
    }
  } catch (TimeLimitExceeded e) {
    cerr << "Search interrupted after reaching time limit of " << time_limit << " milliseconds" << endl;
  }
  return out;
}
