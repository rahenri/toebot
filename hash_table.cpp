#include "hash_table.h"

HashTable::HashTable(int size) {
  this->size = size;
  this->data = new BoardMemo[size];
}

HashTable::~HashTable() {
  delete[] this->data;
}

