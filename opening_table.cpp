#include <iostream>
#include <fstream>

#include "opening_table.h"
#include "ai.h"

using namespace std;

struct OpeningGenerator {
  fstream output;
  HashTable* table;

  OpeningGenerator(HashTable* table) : table(table) {
    output.open("generated_opening_table.cpp", ios::out | ios::trunc);
    output << "#include <vector>" << endl;
    output << endl;
    output << "#include \"generated_opening_table.h\"" << endl;
    output << endl;
    output << "using namespace std" << endl;
    output << endl;
    output << "vector<TableIem> generatedOpeningTable = {" << endl;
  }

  ~OpeningGenerator() {
    output << "};" << endl;
    output.close();
  }


  void RecursiveGenTable(Board* board, int player, int depth) {
    if (depth == 0) {
      return;
    }

    SearchOptions opt;
    opt.time_limit = 10000; // 10s
    opt.use_open_table = false;
    auto result = SearchMove(table, board, player, opt);

    output << "  {0x" << hex << board->Hash() << ", " << result.move_count <<  ", " << "{";
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
  generator.RecursiveGenTable(&board, 1, 4);
}
