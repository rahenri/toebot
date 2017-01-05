#!/usr/bin/env python3
import fileinput

print('#include <unordered_map>')
print('')
print('#include "generated_opening_table.h"')
print('')
print('using namespace std;')
print('')
print('unordered_map<uint64_t, TableItem> generatedOpeningTable = {')

for line in fileinput.input():
  items = line.strip().split(' ')
  hash = items[0]
  moves = items[1:]
  print('  {%s, {%d, {%s}}},' % (hash, len(moves), ', '.join(moves)))

print('};')
