#include <vector>
#include <algorithm>
#include <iostream>

#include "util.h"
#include "board.h"

using namespace std;

struct EncodeCellTest {
  int row;
  int col;
  int cell;
};

bool TestDecodeEncodeCell() {
  vector<EncodeCellTest> tests{
    {0, 0, 0},
    {0, 1, 1},
    {0, 2, 2},
    {0, 3, 9},
    {1, 0, 3},
    {2, 0, 6},
    {3, 0, 27},
  };
  for (auto test : tests) {
    int got = encodeCell(test.row, test.col);
    if (got != test.cell) {
      cerr << "Cell is " << got << " want " << test.cell << endl;
      return false;
    }
    int row, col;
    decodeCell(test.cell, row, col);
    if (row != test.row || col != test.col) {
      cerr << "Cell=" << test.cell << " Coord is " << row << " " << col << " want " << test.row << " " << test.col << " " << endl;
      return false;
    }
  }
  return true;
}

bool TestUnTick() {
  Board clean;
  Board board;
  int info = board.tick(0, 1);
  board.untick(0, info);
  if (clean != board) {
    cerr << "Boards differ";
    return false;
  }

  board.tick(0, 1);
  clean.tick(0, 1);
  info = board.tick(1, 2);
  board.untick(1, info);
  if (clean != board) {
    cerr << "Boards differ";
    return false;
  }
  return true;
}

bool RunTests() {
  auto tests = {
    TestDecodeEncodeCell,
    TestUnTick,
  };
  for (auto test : tests) {
    if (!test()) {
      return false;
    }
  }
  return true;
}

