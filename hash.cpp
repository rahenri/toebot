#include <random>
#include <iostream>

#include "hash.h"

using namespace std;

uint64_t cell_masks[9*9][3];
uint64_t next_masks[10];

void InitHashConstants() {
  mt19937_64 generator;
  generator.seed(0x438ead462650e4a);
  for (int i = 0; i < 9*9; i++) {
    for (int j = 0; j < 3; j++) {
      cell_masks[i][j] = generator();
    }
  }
  for (int i = 0; i < 10; i++) {
    next_masks[i] = generator();
  }
}

uint64_t HashBoard(const Board* board) {
  int next_board = -1;
  for (int i = 0; i < 9; i++) {
    if (board->macrocells[i] == -1) {
      if (next_board == -1) {
        next_board = i;
      } else {
        next_board = 9;
      }
    }
  }
  if (next_board == -1) {
    next_board = 9;
  }
  uint64_t out = next_masks[next_board];
  for (int i = 0; i < 9*9; i++) {
    out ^= cell_masks[i][board->cells[i]];
  }
  if (out == 0) {
    cerr << "WARNING: hash is 0" << endl;
  }
  return out;
}

HashTable::HashTable(int size) {
  this->size = size;
  this->data = new BoardMemo[size];
}

HashTable::~HashTable() {
  delete[] this->data;
}