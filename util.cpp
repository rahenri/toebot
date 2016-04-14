#include "util.h"

vector<string> parseCSV(const string& csv) {
  vector<string> out;
  string part;
  if (csv.size() == 0) {
    return out;
  }
  for (char c : csv) {
    if (c == ',') {
      out.push_back(part);
      part = "";
    } else {
      part += c;
    }
  }
  out.push_back(part);
  return out;
}

