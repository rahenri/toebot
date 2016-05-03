#include <iostream>
#include <fstream>

#include "opening_table.h"
#include "ai.h"

using namespace std;

struct OpeningGenerator {
  fstream output;
  HashTable* table;

  OpeningGenerator(HashTable* table) : table(table) {
    output.open("generated_opening_table.h", ios::out | ios::trunc);
    output << "#ifndef GENERATED_OPENING_TABLE_H" << endl;
    output << "#define GENERATED_OPENING_TABLE_H" << endl;
    output << endl;
    output << "#include <map>" << endl;
    output << "#include <vector>" << endl;
    output << endl;
    output << "std::map<uint64_t, std::vector<int>> GeneratedOpeningTable = {" << endl;
  }

  ~OpeningGenerator() {
    output << "};" << endl;
    output << endl;
    output << "#endif" << endl;
    output.close();
  }


  void RecursiveGenTable(Board* board, int player, int depth) {
    if (depth == 0) {
      return;
    }

    SearchOptions opt;
    opt.time_limit = 5000; // 5s
    opt.use_open_table = false;
    auto result = SearchMove(table, board, player, opt);

    output << "  {0x" << hex << board->Hash() << ", " << "{";
    for (int i = 0; i < result.move_count; i++) {
      if (i > 0) {
        output << ", ";
      }
      output << dec << result.moves[i];
    }
    output << "}}," << endl;
    output << flush;

    uint8_t moves[9*9];
    int count = board->ListMoves(moves, -1);
    for (int i = 0; i < count; i++) {
      board->tick(moves[i], player);
      RecursiveGenTable(board, 3-player, depth-1);
      board->untick(moves[i], player);
    }
  }
};


void GenOpeningTable(HashTable* table) {
  Board board;
  OpeningGenerator generator(table);
  generator.RecursiveGenTable(&board, 1, 2);
}
