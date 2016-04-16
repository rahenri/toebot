#ifndef HASH_H
#define HASH_H

#include "board.h"

void InitHashConstants();

uint64_t HashBoard(const Board* board);

struct BoardMemo {
  uint64_t hash = 0;
  int lower_bound, upper_bound;
  int depth;
  int move;
};

class HashTable {
  public:
    HashTable(int size);
    ~HashTable();

    BoardMemo* Get(const Board* board) {
      uint64_t hash = HashBoard(board);
      BoardMemo* memo = this->data + (hash % this->size);
      if (memo->hash != hash) {
        return nullptr;
      }
      return memo;
    }

    BoardMemo* Insert(const Board *board, int lower_bound, int upper_bound, int depth, int move) {
      uint64_t hash = HashBoard(board);
      BoardMemo* memo = this->data + (hash % this->size);
      if (memo->hash != hash || depth > memo->depth) {
        memo->hash = hash;
        memo->depth = depth;
        memo->lower_bound = lower_bound;
        memo->upper_bound = upper_bound;
        memo->move = move;
      } else if(depth == memo->depth) {
        if (lower_bound > memo->lower_bound) {
          memo->move = move;
        }
        memo->lower_bound = max(memo->lower_bound, lower_bound);
        memo->upper_bound = min(memo->upper_bound, upper_bound);
      }
      return memo;
    }

  private:
    BoardMemo* data;
    int size;
};

#endif
