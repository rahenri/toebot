#include <chrono>
#include <cstring>
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>

#include "ai.h"
#include "board.h"
#include "flags.h"
#include "interruption.h"
#include "line_reader.h"
#include "opening_table.h"
#include "random.h"
#include "score_table.h"
#include "util.h"
#include "cmd_args.h"

using namespace std;
using namespace std::chrono;

const int* MinTimeLimit = NewIntFlag("min-time-limit", 0);
const int* DefaultTimeLimit = NewIntFlag("default-time-limit", 800);

struct Settings {
  int time_bank = 10000;
  int time_per_move = 500;
  vector<string> player_names;
  string my_name;
  int my_id = 1;

  void update(const vector<string>& args) {
    if (args.size() != 2) {
      cerr << "Wrong number of arguments to settings" << endl;
      return;
    }
    string setting = args[0];
    string value = args[1];
    if (setting == "timebank") {
      time_bank = stoi(value);
    } else if(setting == "time_per_move") {
      time_per_move = stoi(value);
    } else if(setting == "player_names") {
      player_names = parseCSV(value);
    } else if(setting == "your_bot") {
      my_name = value;
    } else if(setting == "your_botid") {
      my_id = stoi(value);
    } else {
      cerr << "Unknown setting: " << setting << endl;
    }
  }
};

struct Game {
  int round = 0;
  int move = 0;

  Settings settings;

  Board board;

  int turn = 1;

  bool update(const vector<string>& args) {
    if (args.size() != 3) {
      cerr << "Wrong number of arguments to update game: " << endl;
      return false;
    }
    string name = args[1];
    string value = args[2];
    if (name == "round") {
      round = stoi(value);
    } else if (name == "move") {
      move = stoi(value);
    } else if (name == "field") {
      Board copy = board;
      bool ret = board.ParseBoard(value);
      int diffs = 0;
      int diff = -1;
      int player = 0;
      for (int i = 0; i < 9*9; i++) {
        if (copy.Cell(i) == 0 && board.Cell(i) != 0) {
          diffs ++;
          diff = i;
          player = board.Cell(i);
        } else if (copy.Cell(i) != board.Cell(i)) {
          diffs ++;
        }
      }
      if (diffs == 1 && diff >= 0) {
        cerr << "Opponent played cell " << diff << endl;
        this->turn = player ^ 3;
      } else {
        cerr << "Don't know what the opponent played, diffs: " << diffs << endl;
        this->turn = 2 - (board.ply() % 2);
      }
      return ret;
    } else if (name == "macroboard") {
      return board.ParseMacroBoard(value);
    } else {
      cerr << "Unknown game variable: " << name << endl;
    }
    return true;
  }

  bool handleAction(const vector<string>& args) {
    turn = settings.my_id;
    int time_limit = *DefaultTimeLimit;
    if (args.size() >= 2) {
      time_limit = min(stoi(args[1]) - 50, *DefaultTimeLimit);
    }
    time_limit = max(time_limit, *MinTimeLimit);
    SearchOptions opt;
    opt.time_limit = time_limit;
    SearchResult result = SearchMove(&board, settings.my_id, opt);
    if (result.move_count == 0) {
      cerr << "No cell available" << endl;
      return false;
    }
    int move = result.RandomMove();
    board.tick(move, settings.my_id);
    cerr << "Move Score: " << result.score << ", Nodes: " << result.nodes << " Depth: " << result.depth << " Move: " << move << endl;
    cerr << board;
    cerr << board.BoardRepr() << endl;
    cerr << board.MacroBoardRepr() << endl;
    int row, col;
    decodeCell(move, row, col);
    cout << "place_move " << col << " " << row << endl << flush;
    this->turn ^= 3;
    cerr << "Turn is " << this->turn << endl;
    return true;
  }

  bool opponentMove(int row, int col) {
    int cell = encodeCell(row, col);
    if (!board.canTick(cell)) {
      return false;
    }
    board.tick(cell, settings.my_id^3);
    return true;
  }

  void handleListMoves() {
    for (int i = 0; i < 9*9; i++) {
      if (board.canTick(i)) {
        cerr << i << endl;
      }
    }
  }

  SearchResult Ponder() {
    cerr << "Start pondering" << endl;
    // Ponder for up to 10 minutes.
    SearchOptions opt;
    opt.interruptable = true;
    opt.time_limit = 60000; // 10 min
    auto out = SearchMove(&board, this->turn, opt);
    cerr << "End pondering" << endl;
    return out;
  }
};

bool RunTests();

bool handleSelfPlay() {
  RandSeed(0);
  Board board;
  int player = 1;
  int rounds = 0;
  int64_t total_nodes = 0;
  while(1) {
    auto t1 = steady_clock::now();
    if (board.isOver()) {
      cerr << "Player " << (player^3) << " won" << endl;
      break;
    }
    if (board.IsDrawn()) {
      cerr << "Draw" << endl;
      break;
    }
    SearchOptions opt;
    opt.time_limit = max(*MinTimeLimit, *DefaultTimeLimit);
    SearchResult result = SearchMove(&board, player, opt);
    if (result.signal_interruption) {
      return false;
    }
    if (result.move_count == 0) {
      cerr << "No move found!" << endl;
      break;
    }
    int move = result.RandomMove();
    if (move >= 9*9 || !board.canTick(move)) {
      cerr << "Bot produced invalid move: " << move << endl;
      break;
    }
    board.tick(move, player);
    int row, col;
    decodeCell(move, row, col);
    auto time_span = duration_cast<duration<double>>(steady_clock::now() - t1);
    cout << "place_move " << col << " " << row << endl;
    cerr << result << " Time: " << time_span.count() << " seconds" << endl;
    cerr << board;
    cerr << board.BoardRepr() << endl;
    cerr << board.MacroBoardRepr() << endl;
    cerr << endl;
    player ^= 3;
    rounds++;
    total_nodes += result.nodes;
  }
  cerr << "Rounds: " << rounds << " Total Nodes: " << HumanReadable(total_nodes) << endl;
  return true;
}

int main(int argc, const char** argv) {
  RandSeed(duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count());

  if (!ParseFlags(argc, argv)) {
    return 1;
  }

  InitSignals();
  InitHashConstants();
  InitScoreTable();
  InitBoardConstants();

  string line;
  unique_ptr<Game> game(new Game);

  LineReader* reader = &LineReaderSingleton;

  bool ready_to_ponder = false;

  while (1) {
    bool can_block = (!*EnablePonder) || (!ready_to_ponder);
    int ret = reader->ReadLine(&line, can_block);
    if (ret == 0) {
      cerr << "EOF, goodbye" << endl;
      break;
    }
    if (ret == EAGAIN) {
      // Nothing to read, so ponder if enabled.
      if (*EnablePonder) {
        cerr << "Pondering due to inactivity" << endl;
        auto result = game->Ponder();
        if (result.signal_interruption) {
          ready_to_ponder = false;
          cerr << "Pondering paused due to SIGINT" << endl;
        }
        if (!result.input_interruption) {
          ready_to_ponder = false;
          cerr << "Pondering finished naturally" << endl;
        }
      }
      continue;
    }
    if (ret < 0) {
      if (InterruptRequested()) {
        cerr << "Interruption requested, goodbye" << endl;
        break;
      }
      cerr << "IO interrupted by signal" << endl;
      continue;
    }

    cerr << line << endl;

    steady_clock::time_point t1 = steady_clock::now();
    vector<string> command = parseLine(line);
    if (command.size() == 0) {
      continue;
    }
    string name = command[0];
    vector<string> args = sliceVector(command, 1);
    bool success = true;
    if (name == "action") {
      success = game->handleAction(args);
      ready_to_ponder = true;
    } else if (name == "settings") {
      game->settings.update(args);
    } else if (name == "update") {
      success = game->update(args);
    } else if (name == "test") {
      bool ok = RunTests();
      if (ok) {
        cerr << "Tests pass" << endl;
      } else {
        cerr << "Tests failed" << endl;
      }
    } else if (name == "move") {
      int row = stoi(args[0]);
      int col = stoi(args[1]);
      if (!game->opponentMove(row, col)) {
        cerr << "Move failed" << endl;
        continue;
      } else {
        cerr << game->board;
      }
    } else if (name == "self_play") {
      if (!handleSelfPlay()) {
        break;
      }
    } else if (name == "list_moves") {
      game->handleListMoves();
    } else if (name == "gen_opening") {
      GenOpeningTable();
    } else {
      cerr << "Unknown command: " << name << endl;
      success = false;
    }
    if (!success) {
      cerr << "Command failed: " << line << endl;
    }
    steady_clock::time_point t2 = steady_clock::now();
    duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
    cerr << "Command took " << (time_span.count() * 1000.0) << " milliseconds." << endl;
  }

  return 0;
}
