#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <cstdint>
#include <unordered_map>

#include "board.h"
#include "flags.h"

using namespace std;

static const int HashTableSize = 50000017;
// static const int HashTableSize = 200000033;

struct BoardMemo {
  uint64_t hash;
  int32_t lower_bound, upper_bound;
  int depth;
  int move;
};

struct TopLevelSlot {
  int32_t score = 0;
  int depth = -1;
  int moves[81];
  int move_count = 0;
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

class TopLevelHashTable {
  public:
    TopLevelSlot* Get(const Board* board) {
      uint64_t hash = board->Hash();
      auto it = data.find(hash);
      if (it == data.end()) {
        return nullptr;
      }
      TopLevelSlot* slot = &(it->second);
      return slot;
    }

    TopLevelSlot* Insert(const Board *board, int score, int depth, int* moves, int move_count) {
      TopLevelSlot* slot = Get(board);
      if (slot == nullptr) {
        auto it = data.insert(make_pair(board->Hash(), TopLevelSlot()));
        slot = &(it.first->second);
      }
      if (depth > slot->depth) {
        slot->depth = depth;
        slot->score = score;
        slot->move_count = move_count;
        for (int i = 0 ; i < move_count; i++) {
          slot->moves[i] = moves[i];
        }
      }
      return slot;
    }

  private:
    unordered_map<uint64_t, TopLevelSlot> data;
};

extern HashTable HashTableSingleton;

extern TopLevelHashTable TopLevelHashTableSingleton;


#endif
