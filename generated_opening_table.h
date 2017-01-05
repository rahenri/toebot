#ifndef GENERATED_OPENING_TABLE_H
#define GENERATED_OPENING_TABLE_H

#include "board.h"

#include <unordered_map>
#include <cstdint>

struct TableItem {
  int8_t move_count;
  int8_t moves[4];
};

extern std::unordered_map<uint64_t, TableItem> generatedOpeningTable;

void InitOpeningTable();

#endif
