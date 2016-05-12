#ifndef UTIL_H
#define UTIL_H

#include <vector>
#include <string>

using namespace std;

typedef int cell;
typedef int macrocell;

inline int encodeCell(int row, int col) {
  int mrow = row / 3;
  int mcol = col / 3;
  row %= 3;
  col %= 3;
  return (mrow*3+mcol)*9 + (row*3+col);
}

inline void decodeCell(cell c, int& row, int& col) {
  macrocell mc = c / 9;
  int mrow = mc / 3;
  int mcol = mc % 3;
  int offset = c % 9;
  row = mrow*3 + (offset/3);
  col = mcol*3 + (offset%3);
}

template <class T>
string HumanReadable(T number) {
  bool neg = number < 0;
  if (neg) number = -number;
  string output;
  for (int i = 0; number > 0; i++) {
    if (i % 3 == 0 && i > 0) {
      output = "," + output;
    }
    output = char(number%10 + '0') + output;
    number /= 10;
  }
  return output;
}

vector<string> parseCSV(const string& csv);

#endif
