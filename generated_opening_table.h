#ifndef GENERATED_OPENING_TABLE_H
#define GENERATED_OPENING_TABLE_H

#include <vector>
#include <cstdint>

struct TableItem {
  uint64_t hash;
  int move_count;
  int8_t moves[81];
};

extern std::vector<TableItem> generatedOpeningTable;

inline TableItem* FindOpeningTable(uint64_t hash) {
  int begin = 0, end = generatedOpeningTable.size();
  while (begin < end) {
    int middle = (begin + end) / 2;
    auto item = &generatedOpeningTable[middle];
    if (item->hash == hash) {
      return item;
    }
    if (item->hash < hash) {
      begin = middle + 1;
    }
    if (item->hash > hash) {
      end = middle;
    }
  }
  return nullptr;
}

#endif
