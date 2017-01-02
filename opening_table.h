#ifndef OPENING_TABLE_H
#define OPENING_TABLE_H

#include "generated_opening_table.h"

class HashTable;

void GenOpeningTable();

TableItem* FindOpeningTable(const Board& board);

pair<uint64_t, TableItem> OpeningTableHash(const Board& board, const TableItem& item);

void InsertOpeningTable(const Board& board, const TableItem& item);

void DumpOpeningTable();

#endif

