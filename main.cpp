#include <iostream>
#include <sstream>
#include <vector>

#include "util.h"

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

vector<string> parseCSV(const string& csv) {
  vector<string> out;
  string part;
  if (csv.size() == 0) {
    return out;
  }
  for (char c : csv) {
    if (c == ',') {
      out.push_back(part);
      part = "";
    } else {
      part += c;
    }
  }
  out.push_back(part);
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

  int board[9*9];
  int macroboard[3*3];

  void parseBoard(const string& repr) {
    if (repr.size() != 81*2-1) {
      cerr << "Bad board repr size:" << repr.size() << endl;
      return;
    }
    for (int i = 0; i < 9; i++) {
      for (int j = 0; j < 9; j++) {
        int k = i*9+j;
        board[encodeCell(i, j)] = repr[k*2] - '0';
      }
    }
  }

  void parseMacroBoard(const string& repr) {
    auto parts = parseCSV(repr);
    if (parts.size() != 9) {
      cerr << "Bad macro board size: " << parts.size() << endl;
      return;
    }
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        int k = i*3+j;
        macroboard[k] = stoi(parts[k]);
      }
    }
  }

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
      parseBoard(value);
    } else if (name == "macroboard") {
      parseMacroBoard(value);
    } else {
      cerr << "Unknown game variable: " << name << endl;
    }
  }

  cell firstCell() {
    macrocell forced = -1;
    for (int i = 0; i < 9; i++) {
      if (macroboard[i] == -1) {
        forced = i;
        break;
      }
    }
    if (forced >= 0) {
      int base = forced * 9;
      for (int i = base; i < base + 9; i++) {
        if (board[i] == 0) {
          return i;
        }
      }
    }
    for (int i = 0; i < 9; i++) {
      if (macroboard[i] != 0) {
        continue;
      }
      int base = i * 9;
      for (int i = base; i < base + 9; i++) {
        if (board[i] == 0) {
          return i;
        }
      }
    }
    return -1;
  }
};

bool RunTests();

int main() {
  string line;
  settings settings;
  game game;
  while (getline(cin, line)) {
    vector<string> command = parseLine(line);
    if (command.size() == 0) {
      continue;
    }
    string name = command[0];
    vector<string> args = sliceVector(command, 1);
    if (name == "action") {
      cell c = game.firstCell();
      if (c == -1) {
        cerr << "No cell available" << endl;
        continue;
      }
      int row, col;
      decodeCell(c, row, col);
      cout << "place_move " << col << " " << row << endl;
    } else if (name == "settings") {
      settings.update(args);
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
