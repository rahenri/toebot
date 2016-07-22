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
auto DepthShortening = NewIntFlag("depth-shortening", 4);
auto ShorteningThreshold = NewIntFlag("shortening-threshold", 50000);

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

// The AI class contains all state being used when search the tree of moves.
class AI {
 public:

   // Initialize the AI to search a specific board with the the given params
   // - board: the initial state of the board
   // - player: the player to play next
   // - ply: how many moves have been playes so far in the game.
   // - time_limit: specifies the maximum amount of time to spend on the search in milliseconds, no time limit if zero.
   // - interruptable: whether the search can be interrupted if the is any data available in the input, used for pondering.
  AI(const Board* board, int player, int ply, int time_limit, bool interruptable)
    : interruptable(interruptable), board(*board), player(player), ply(ply), depth_shortening(*DepthShortening), shortening_threshold(*ShorteningThreshold) {

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

  // Set the maximum search depth, used for iterative deepening search, where a
  // sequence of searches is performed from the same position at deeper and
  // deeper depth.
  void SetMaxDepth(int depth) {
    this->depth = depth;
  }

  // Search the best move from the given paramters given in the constructor.
  // It can throw exception TimeLimitExceeded if time spent exeeced the time
  // limit given in the constricutor.
  // It can throw exception InterrutionRequestedException if interruptable is
  // true and there is data available to be read from the input.
  // This functions is similar to DeepEval, the difference is that there is
  // some differences on the things that needs to be done on the root of the
  // search tree compared to the rest. Deduping repeated logic by refactoring
  // them out into functions or merging these functions proved to be less
  // efficient.
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

 private:

  // Checks whether the search should be interrupted, the appropriate exception
  // is thrown if so.
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

  // Recursive call for searching the tree, it updates all internal state to
  // search the next position and revert those updates at the end. This is done
  // to avoid copying or a very long list of function arguments that would be
  // less efficient.
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
    if (depth_shortening > 0 && !shortened && depth >= depth_shortening) {
      shortened = true;
      depth -= depth_shortening;
      score = -this->DeepEval(-(beta+shortening_threshold), -(alpha-shortening_threshold));
      depth += depth_shortening;
      shortened = false;
      if  (score < alpha - shortening_threshold || score > beta + shortening_threshold) {
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

  // Performs the actual search in the move tree, it returns the score of the best move found.
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

  int depth_shortening;
  int shortening_threshold;

  SearchTreePrinter* printer;

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

// Compute the best move for the given board and player.
// This is the entry point for the AI search.
SearchResult SearchMove(const Board *board, int player, SearchOptions opt) {
  int ply = 1;
  for (int i = 0; i < 9*9; i++) {
    if (board->Cell(i) != 0) {
      ply ++;
    }
  }

  SearchResult out;

  // Lookup opening table, return a move from there if we find a hit.
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
          cerr << "Invalid move from opening table: " << m << endl;
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

  // Iterative deepening search. It is better than fixed deptch because of time
  // constraints, ie, fixed depth can take a variable amount of depth. It is
  // also better to populate the cache with good moves, which improves
  // alpha-beta search.
  for (int depth = 2; depth <= *MaxDepth; depth ++) {
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
