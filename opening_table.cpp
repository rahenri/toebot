#include <iostream>
#include <fstream>
#include <set>

#include "opening_table.h"
#include "ai.h"

using namespace std;

struct OpeningGenerator {
  fstream output;

  OpeningGenerator() {
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


  void RecursiveGenTable(Board* board, int depth) {
    if (depth == 0) {
      return;
    }

    SearchOptions opt;
    opt.time_limit = 10000; // 10s
    opt.use_open_table = false;
    auto result = SearchMove(board, opt);

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
      auto info = board->tick(moves[i]);
      RecursiveGenTable(board, depth-1);
      board->untick(moves[i], info);
    }
  }
};

void DumpOpeningTable() {
  fstream output;

  output.open("generated_opening_table.cpp", ios::out | ios::trunc);
  output << "#include <vector>" << endl;
  output << endl;
  output << "#include \"generated_opening_table.h\"" << endl;
  output << endl;
  output << "using namespace std" << endl;
  output << endl;
  output << "vector<TableIem> generatedOpeningTable = {" << endl;



  set<uint64_t> hashes;
  for (const auto& item : generatedOpeningTable) {
    hashes.insert(item.first);
  }

  for (uint64_t hash : hashes) {
    auto item = generatedOpeningTable.find(hash)->second;
    output << "  {0x" << hex << hash << ", {" << item.move_count <<  ", " << "{";
    for (int i = 0; i < item.move_count; i++) {
      if (i > 0) {
        output << ", ";
      }
      output << dec << item.moves[i];
    }
    output << "}}}," << endl;
    output << flush;
  }

  output << "};" << endl;
  output.close();
};


void GenOpeningTable() {
  Board board;
  OpeningGenerator generator;
  generator.RecursiveGenTable(&board, 4);
}

void MirrorItem(TableItem* item) {
  for (int i = 0; i < item->move_count;i ++) {
    int r, c;
    decodeCell(item->moves[i], r, c);
    c = 8 - c;
    item->moves[i] = encodeCell(r, c);
  }
}

void RotateItem(TableItem* item) {
  for (int i = 0; i < item->move_count;i ++) {
    int r, c;
    decodeCell(item->moves[i], r, c);
    int nr = c;
    int nc = 8 - r;
    item->moves[i] = encodeCell(nr, nc);
  }
}

pair<uint64_t, TableItem> OpeningTableHash(const Board& board, const TableItem& item) {
  Board copy = board;
  TableItem item_copy = item;
  uint64_t hash = copy.Hash();
  TableItem ret = item_copy;
  for (int i = 0; i < 4; i++) {
    copy.Rotate();
    RotateItem(&item_copy);
    for (int j = 0; j < 2; j++) {
      copy.Mirror();
      MirrorItem(&item_copy);
      uint64_t h = copy.Hash();
      if (h < hash) {
        hash = h;
        ret = item_copy;
      }
    }
  }
  return make_pair(hash, ret);
}

uint64_t OpeningTableHash(const Board& board) {
  Board copy = board;
  uint64_t hash = copy.Hash();
  for (int i = 0; i < 4; i++) {
    copy.Rotate();
    for (int j = 0; j < 2; j++) {
      copy.Mirror();
      uint64_t h = copy.Hash();
      if (h < hash) {
        hash = h;
      }
    }
  }
  return hash;
}

TableItem RotateToHash(const Board& board, uint64_t hash, const TableItem& item) {
  Board copy = board;
  TableItem item_copy = item;
  for (int i = 0; i < 4; i++) {
    copy.Rotate();
    RotateItem(&item_copy);
    for (int j = 0; j < 2; j++) {
      copy.Mirror();
      MirrorItem(&item_copy);
      if (copy.Hash() == hash) {
        return item_copy;
      }
    }
  }
  assert(false);
}

TableItem* FindOpeningTable(const Board& board) {
  auto hash = OpeningTableHash(board);
  auto it = generatedOpeningTable.find(hash);
  if (it == generatedOpeningTable.end()) {
    return nullptr;
  }
  return &it->second;
}

void InsertOpeningTable(const Board& board, const TableItem& item) {
  auto pair = OpeningTableHash(board, item);
  generatedOpeningTable.insert(pair);
}

