#include <iostream>
#include <sstream>
#include <vector>

#include "util.h"
#include "board.h"
#include "ai.h"

using namespace std;

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

struct settings {
  int time_bank;
  int time_per_move;
  vector<string> player_names;
  string my_name;
  int my_id;

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

struct game {
  int round;
  int move;

  settings settings;

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
    return SearchMove(&board, settings.my_id);
  }
};

bool RunTests();

int main() {
  string line;
  game game;
  while (getline(cin, line)) {
    vector<string> command = parseLine(line);
    if (command.size() == 0) {
      continue;
    }
    string name = command[0];
    vector<string> args = sliceVector(command, 1);
    if (name == "action") {
      SearchResult result = game.bestCell();
      if (result.move == -1) {
        cerr << "No cell available" << endl;
        continue;
      }
      cerr << "Move Score: " << result.score << endl;
      int row, col;
      decodeCell(result.move, row, col);
      cout << "place_move " << col << " " << row << endl;
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
    } else {
      cerr << "Unknown command: " << name << endl;
    }
  }

  return 0;
}
