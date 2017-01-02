#!/usr/bin/env python3

import sqlite3
import json
import os
import sys

def rotate_field(field):
  out = [0]*81
  for i, v in enumerate(field):
    r = i // 9
    c = i % 9
    out[(8 - r) + 9 * c] = v
  return out

def mirror_field(field):
  out = [0]*81
  for i, v in enumerate(field):
    r = i // 9
    c = i % 9
    out[(8 - c) + 9 * r] = v
  return out


def min_rotation(field):
  ret = field
  ret_tup = tuple(field)
  for _ in range(4):
    field = rotate_field(field)
    for _ in range(2):
      field = mirror_field(field)
      tup = tuple(field)
      if tup < ret_tup:
        ret_tup = tup
        ret = field
  return ret

def field_id(field):
  return ','.join(str(c) for c in min_rotation(field))

def main():
  conn = sqlite3.connect('data.db')

  outputs = [open('shard{}.in'.format(f), 'w') for f in range(int(sys.argv[1]))]
  try:

    seen = {}
    query = conn.execute('SELECT data from game_urls WHERE data != "" and data != "gone"')
    game = 0
    for data_str, in query:
      game += 1
      data = json.loads(data_str)
      for state in data['states'][:12:2]:
        field_str = state['field']
        field = [int(cell) for cell in field_str.split(',')]
        fid = field_id(field)
        if fid in seen:
          seen[fid] += 1
          continue
        seen[fid] = 1
        board = [0] * 81
        macroboard = [0] * 9
        turn = int(state['player'])
        for i, cell in enumerate(field):
          mcell = ((i // 3) % 3) + 3 * (i // 27)
          if cell&1:
            board[i] = 1
          if cell&2:
            board[i] = 2
          if (cell&4) or (cell&8):
            macroboard[mcell] = -1
          if (cell&16):
            macroboard[mcell] = 1
          if (cell&32):
            macroboard[mcell] = 2
        board_str = ','.join(str(cell) for cell in board)
        macroboard_str = ','.join(str(cell) for cell in macroboard)

        output = ''.join((
            'update game field {}\n'.format(board_str),
            'update game macroboard {}\n'.format(macroboard_str),
            'update game turn {}\n'.format(turn),
            'gen_ot_entry\n'))

        out_id = game % len(outputs)
        outputs[out_id].write(output)
  finally:
    for out in outputs:
      out.close()





if __name__ == '__main__':
  main()
