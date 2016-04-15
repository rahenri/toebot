#include <iostream>
#include <sstream>
#include <vector>
#include <chrono>

#include "util.h"
#include "board.h"
#include "ai.h"

using namespace std;
using namespace std::chrono;

vector<string> parseLine(const string& line) {
  vector<string> out;
  stringstream ss(line);
  string part;
  while (ss >> part) {
    if (part.size() > 0 && part[0] == '#') {
      break;
    }
    out.push_back(part);
  }
  return out;
}

template <class T>
vector<T> sliceVector(const vector<T>& input, int start = 0, int end = -1) {
  vector<T> out;
  if (end == -1 || end > input.size()) {
    end = input.size();
  }
  for (; start < end; start++) {
    out.push_back(input[start]);
  }
  return out;
}

struct Settings {
  int time_bank = 0;
  int time_per_move = 0;
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

  void update(const vector<string>& args) {
    if (args.size() != 3) {
      cerr << "Wrong number of arguments to update game" << endl;
      return;
    }
    string name = args[1];
    string value = args[2];
    if (name == "round") {
      round = stoi(value);
    } else if (name == "move") {
      move = stoi(value);
    } else if (name == "field") {
      parseBoard(&board, value);
    } else if (name == "macroboard") {
      parseMacroBoard(&board, value);
    } else {
      cerr << "Unknown game variable: " << name << endl;
    }
  }

  SearchResult bestCell() {
    SearchResult result =  SearchMove(&board, settings.my_id);
    board.tick(result.move, settings.my_id);
    return result;
  }

  void handleAction() {
    SearchResult result = bestCell();
    if (result.move == -1) {
      cerr << "No cell available" << endl;
      return;
    }
    board.tick(result.move, settings.my_id);
    cerr << "Move Score: " << result.score << ", Nodes: " << result.nodes << endl;
    cerr << board;
    int row, col;
    decodeCell(result.move, row, col);
    cout << "place_move " << col << " " << row << endl;
  }

  bool opponentMove(int row, int col) {
    int cell = encodeCell(row, col);
    if (!board.canTick(cell)) {
      return false;
    }
    board.tick(cell, 3-settings.my_id);
    return true;
  }
};

bool RunTests();

void handleSelfPlay() {
  Board board;
  int player = 1;
  int rounds = 0;
  while(1) {
    if (board.isOver()) {
      cerr << "Player " << (3-player) << " won" << endl;
      break;
    }
    if (board.isDrawn()) {
      cerr << "Draw" << endl;
      break;
    }
    SearchResult result = SearchMove(&board, player);
    if (result.move == -1) {
      cerr << "No move found!" << endl;
      break;
    }
    if (!board.canTick(result.move)) {
      cerr << "Bot produced invalid move" << endl;
      break;
    }
    board.tick(result.move, player);
    int row, col;
    decodeCell(result.move, row, col);
    cout << "place_move " << col << " " << row << endl;
    cerr << "Move Score: " << result.score << ", Nodes: " << result.nodes << endl;
    cerr << board;
    cerr << board.BoardRepr() << endl;
    cerr << board.MacroBoardRepr() << endl;
    cerr << endl;
    player = 3-player;
    rounds++;
  }
  cerr << "Rounds: " << rounds << endl;
}

int main() {
  string line;
  Game game;
  while (getline(cin, line)) {
    steady_clock::time_point t1 = steady_clock::now();
    vector<string> command = parseLine(line);
    if (command.size() == 0) {
      continue;
    }
    string name = command[0];
    vector<string> args = sliceVector(command, 1);
    if (name == "action") {
      game.handleAction();
    } else if (name == "settings") {
      game.settings.update(args);
    } else if (name == "update") {
      game.update(args);
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
      if (!game.opponentMove(row, col)) {
        cerr << "Move failed" << endl;
        continue;
      } else {
        cerr << game.board;
      }

      game.handleAction();
    } else if (name == "self_play") {
      handleSelfPlay();
    } else {
      cerr << "Unknown command: " << name << endl;
    }
    steady_clock::time_point t2 = steady_clock::now();
    duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
    cerr << "Command took " << (time_span.count() * 1000.0) << " milliseconds." << endl;
  }

  return 0;
}
