#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <cstdint>

#include "board.h"
#include "flags.h"

static const int HashTableSize = 50000017;
// static const int HashTableSize = 200000033;

struct BoardMemo {
  uint64_t hash;
  int32_t lower_bound, upper_bound;
  int depth;
  int move;
};

class HashTable {
  public:
    BoardMemo* Get(const Board* board) {
      uint64_t hash = board->Hash();
      BoardMemo* memo = this->data + (hash % HashTableSize);
      if (memo->hash != hash) {
        return nullptr;
      }
      return memo;
    }

    BoardMemo* Insert(const Board *board, int lower_bound, int upper_bound, int depth, int move) {
      uint64_t hash = board->Hash();
      BoardMemo* memo = this->data + (hash % HashTableSize);
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
    BoardMemo data[HashTableSize];
};

extern HashTable HashTableSingleton;


#endif
