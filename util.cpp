#include "util.h"

#include <sstream>

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

vector<string> parseLine(const string& line) {
  vector<string> out;
  stringstream ss(line);
  string part;
  while (ss >> part) {
    if (part.size() > 0 && part[0] == '#') {
      break;
    }
    out.push_back(part);
  }
  return out;
}

