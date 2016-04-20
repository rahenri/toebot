#ifndef HASH_H
#define HASH_H

class Board;

extern uint64_t cell_masks[9*9][3];
extern uint64_t next_masks[10];

void InitHashConstants();

uint64_t HashBoard(const Board* board);

inline uint64_t UpdateHash(uint64_t hash, int next_macro, int prev_next_macro, int cell, int player) {
  return hash
    ^ next_masks[prev_next_macro]
    ^ next_masks[next_macro]
    ^ cell_masks[cell][0]
    ^ cell_masks[cell][player];
}

#endif
