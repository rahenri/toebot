#include <iostream>

#include "search_tree_printer.h"
#include "hash.h"

using namespace std;

SearchTreePrinter::SearchTreePrinter() {
  output.open("tree.txt", ios::out | ios::trunc);
}

SearchTreePrinter::~SearchTreePrinter() {
  output.close();
}

void SearchTreePrinter::Push(const Board* board) {
  uint64_t hash = HashBoard(board);
  output << indent << hash << " {" << endl;
  depth++;

  indent += "  ";
}

void SearchTreePrinter::Pop() {
  depth--;

  indent.pop_back();
  indent.pop_back();

  output << indent << "}" << endl;
}
