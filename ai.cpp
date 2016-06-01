#include <algorithm>
#include <chrono>
#include <iostream>
#include <memory>

#include "ai.h"
#include "flags.h"
#include "hash.h"
#include "interruption.h"
#include "random.h"
#include "search_tree_printer.h"
#include "flags.h"
#include "generated_opening_table.h"
#include "line_reader.h"
#include "hash_table.h"
#include "cmd_args.h"

using namespace std::chrono;

static const int DrawPenalty = 50;

static const int HashMinDepth = 2;

static const bool PrintSearchTree = false;
auto DepthShortening = NewIntFlag("depth-shortening", 0);
auto ShorteningThreshold = NewIntFlag("shorterning-threshold", 20000);

const int* MaxDepth = NewIntFlag("max-depth", 100);

const bool* AnalysisMode = NewBoolFlag("analysis", false);

// Disable opening table for now.
const bool* EnableOpeningTable = NewBoolFlag("enable-opening-table", false);

struct TimeLimitExceeded {
};

struct InterrutionRequestedException {
};

unique_ptr<SearchTreePrinter> search_tree_printer_single;

SearchTreePrinter* tree_printer() {
  if (search_tree_printer_single == nullptr) {
    search_tree_printer_single.reset(new SearchTreePrinter);
  }
  return search_tree_printer_single.get();
}

struct AI {
  int64_t nodes = 0;
  int time_limit = 0;
  int deadline_counter = 0;
  int initial_player = 0;
  steady_clock::time_point deadline;
  bool has_deadline;
  bool interruptable;
  Board board;
  int player;
  int ply = 0;
  int depth = 0;
  bool shortened = false;

  SearchTreePrinter* printer;

  AI(const Board* board, int player, int ply, int time_limit, bool interruptable)
    : interruptable(interruptable), board(*board), player(player), ply(ply) {

    if (time_limit == 0) {
      this->has_deadline = false;
    } else {
      auto now = steady_clock::now();
      this->deadline = now + milliseconds(time_limit);
      this->has_deadline = true;
    }

    if (PrintSearchTree) {
      printer = tree_printer();
    }
  }

  void SetMaxDepth(int depth) {
    this->depth = depth;
  }

  void checkInterruption() {
    this->deadline_counter++;
    if (this->deadline_counter % (1<<14) != 0) {
      return;
    }
    if (InterruptRequested()) {
      throw InterrutionRequestedException();
    }
    if (*EnablePonder && interruptable) {
      if (LineReaderSingleton.HasData()) {
        throw InterrutionRequestedException();
      }
    }
    if (this->has_deadline) {
      auto now = steady_clock::now();
      if (now >= this->deadline) {
        throw TimeLimitExceeded();
      }
    }
  }

  inline int DeepEvalRec(int cell, int alpha, int beta) {
    auto tick_info = board.tick(cell, player);
    player = 3-player;
    depth--;
    ply++;
    if (PrintSearchTree) {
      printer->Push(&board);
      printer->Attr("player", player);
      printer->Attr("ply", ply);
      printer->Attr("depth", depth);
      printer->Attr("alpha", alpha);
      printer->Attr("beta", beta);
      printer->Attr("board", board.BoardRepr());
      printer->Attr("macro", board.MacroBoardRepr());
    }
    int score;
    bool full_search = true;
    if (*DepthShortening > 0 && !shortened && depth >= *DepthShortening) {
      shortened = true;
      depth -= *DepthShortening;
      score = -this->DeepEval(-(beta+*ShorteningThreshold), -(alpha-*ShorteningThreshold));
      depth += *DepthShortening;
      shortened = false;
      if  (score < alpha - *ShorteningThreshold || score > beta + *ShorteningThreshold) {
        full_search = false;
      }
    }
    if (full_search) {
      score = -this->DeepEval(-beta, -alpha);
    }
    if (PrintSearchTree) {
      printer->Attr("score", score);
      printer->Pop();
    }
    player = 3-player;
    depth++;
    ply--;
    board.untick(cell, tick_info);
    return score;
  }

  inline int DeepEval(int alpha, int beta) {
    this->nodes++;
    this->checkInterruption();

    if (board.isOver()) {
      return -MaxScore + ply;
    }

    if (board.IsDrawn()) {
      return DrawPenalty;
    }

    {
      int upper_bound = MaxScore - (ply + 1);
      if (upper_bound < alpha) {
        return upper_bound;
      }
      int lower_bound = -MaxScore + (ply + 2);
      if (lower_bound > beta) {
        return lower_bound;
      }
    }

    int first_cell = -1;
    if (depth >= HashMinDepth) {
      auto memo = HashTableSingleton.Get(&board);
      if (memo != nullptr) {
        if (PrintSearchTree) {
          printer->Attr("hash_hit", true);
          printer->Attr("hash_lower_bound", memo->lower_bound);
          printer->Attr("hash_upper_bound", memo->upper_bound);
        }
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
    }

    int move_count = 0;
    uint8_t moves[9*9];
    int best_score = -MaxScore;

    if (depth <= 0) {
      if (PrintSearchTree) {
        printer->Attr("leaf", true);
      }
      best_score = board.Eval(player);
      if (best_score > beta) {
        return best_score;
      }
      move_count = board.ListCaptureMoves(moves, player);
    } else {
      move_count = board.ListMoves(moves, first_cell);
    }

    int best_move = -1;
    for (int i = 0; i < move_count; i++) {
      int cell = moves[i];

      int score = this->DeepEvalRec(cell, max(alpha, best_score+1), beta);
      if (score > best_score) {
        best_score = score;
        best_move = cell;
        if (score > beta) {
          break;
        }
      }
    }
    if (depth >= HashMinDepth) {
      int lower_bound = -MaxScore;
      int upper_bound = MaxScore;
      if (best_score > beta) {
        lower_bound = best_score;
      } else if (best_score < alpha) {
        upper_bound = best_score;
      } else {
        lower_bound = upper_bound = best_score;
      }
      HashTableSingleton.Insert(&board, lower_bound, upper_bound, depth, best_move);
    }
    return best_score;
  }

  SearchResult SearchMove() {
    if (PrintSearchTree) {
      printer->Push(&board);
      printer->Attr("player", player);
      printer->Attr("ply", ply);
      printer->Attr("depth", depth);
      printer->Attr("board", board.BoardRepr());
      printer->Attr("macro", board.MacroBoardRepr());
    }
    this->initial_player = player;
    SearchResult out;
    out.score = -MaxScore;
    out.depth = depth;
    out.move_count = 0;

    this->nodes++;

    int first_cell = -1;
    auto memo = HashTableSingleton.Get(&board);
    if (memo != nullptr) {
      first_cell = memo->move;
    }

    if (*AnalysisMode) {
      cerr << "-------------------------------" << endl;
      cerr << "Depth: " << depth << endl;
    }

    uint8_t moves[9*9];
    int move_count = board.ListMoves(moves, first_cell);

    for (int i = 0; i < move_count; i++) {
      int cell = moves[i];
      int alpha = *AnalysisMode ? -MaxScore : out.score;
      int score = this->DeepEvalRec(moves[i], alpha, MaxScore);
      if (*AnalysisMode) {
        cerr << cell << ": " << score << endl;
      }
      if (score > out.score || out.move_count == 0) {
        out.score = score;
        out.moves[0] = cell;
        out.move_count = 1;
      } else if (score == out.score) {
        out.moves[out.move_count++] = cell;
      }
    }
    HashTableSingleton.Insert(&board, out.score, out.score, depth, out.moves[0]);

    out.nodes = nodes;
    sort(out.moves, out.moves+out.move_count);

    if (PrintSearchTree) {
      printer->Pop();
    }
    return out;
  }
};

std::ostream& operator<<(std::ostream& stream, const SearchResult& res) {
  stream << "Move: [";
  for (int i = 0; i < res.move_count; i++) {
    if (i > 0) {
      stream << ", ";
    }
    stream << res.moves[i];
  }
  stream << "] Score: " << res.score << " Depth: " << res.depth << " Nodes: " << HumanReadable(res.nodes);
  return stream;
}

int SearchResult::RandomMove() const {
  return moves[RandN(move_count)];
}

SearchResult SearchMove(const Board *board, int player, SearchOptions opt) {
  int ply = 1;
  for (int i = 0; i < 9*9; i++) {
    if (board->Cell(i) != 0) {
      ply ++;
    }
  }

  SearchResult out;

  // Lookup opening table;
  if (*EnableOpeningTable && opt.use_open_table) {
    auto item = FindOpeningTable(board->Hash());
    if (item) {
      out.nodes = 0;
      out.depth = 0;
      out.score = 0;
      out.move_count = 0;
      for (int i = 0; i < item->move_count; i++) {
        int m = item->moves[i];
        if (!board->canTick(m)) {
          cerr << "Invalied move from opening table: " << m << endl;
          continue;
        }
        out.moves[out.move_count++] = m;
      }
      if (out.move_count > 0) {
        return out;
      }
    }
  }

  AI ai(board, player, ply, opt.time_limit, opt.interruptable);
  out.move_count = 0;
  auto start = steady_clock::now();
  for (int depth = 2; depth <= *MaxDepth; depth += 1) {
    SearchResult tmp;
    try {
      ai.SetMaxDepth(depth);
      tmp = ai.SearchMove();
    } catch (TimeLimitExceeded e) {
      cerr << "Search interrupted after reaching time limit of " << opt.time_limit << " milliseconds" << endl;
      out.time_limit_exceeded = true;
      break;
    } catch (InterrutionRequestedException e) {
      cerr << "Search manually interrupted" << endl;
      out.manual_interruption = true;
      break;
    }
    cerr << tmp;
    if (*AnalysisMode) {
      cerr << " Time: " << duration_cast<milliseconds>(steady_clock::now() - start).count();
    }
    cerr << endl;
    out = tmp;
  }
  return out;
}
