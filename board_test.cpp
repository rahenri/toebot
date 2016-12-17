#include <string>
#include <iostream>

#include "board.h"

using namespace std;

int main() {
  InitScoreTable();

  string field = "1,0,0,2,0,0,0,1,0,1,0,0,2,0,1,0,0,0,0,2,1,2,2,0,0,0,0,0,1,2,2,0,0,0,1,0,1,0,2,0,1,0,2,0,2,0,1,0,0,0,0,0,1,0,1,0,0,2,1,0,0,2,0,0,0,0,2,0,1,0,0,0,0,0,0,0,0,0,0,0,0";

  Board board;
  board.ParseBoard(field);

  cout << board.Heuristic() << endl;

  cout << board.Eval(1) << endl;

  cout << board.Eval(2) << endl;

  return 0;
}

