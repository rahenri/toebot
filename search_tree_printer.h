#ifndef SEARCH_TREE_PRINTER_H
#define SEARCH_TREE_PRINTER_H

#include <string>
#include <iostream>
#include <fstream>

#include "board.h"

class SearchTreePrinter {
  public:

    SearchTreePrinter();
    ~SearchTreePrinter();

    void Push(const Board* board);
    template <class T>
    void Attr(const string& name, const T& value) {
      output << indent << name << ": " << value << std::endl;
    }
    void Pop();

  private:
    int depth = 0;
    std::string indent;
    std::ofstream output;
};

#endif
