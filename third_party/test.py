#!/usr/bin/env python3
# coding=utf-8

from __future__ import division

import argparse
import json
import logging
import math
import multiprocessing
import os
import random
import sys
import time
import traceback

from subprocess import Popen, PIPE, STDOUT
from multiprocessing import Pool

from datetime import datetime

class Estimator:
  def __init__(self):
    self.elements = []

  def add(self, item):
    self.elements.append(item)

  def mean(self):
    l = len(self)
    if l == 0:
      return 0
    return sum(self.elements) / l

  def mean(self):
    l = len(self)
    return sum(self.elements) / l if l else 0.0

  def variance(self):
    l = len(self)
    if l == 0:
      return 0.0
    m = self.mean()
    out = 0.0
    for i in self.elements:
      d = (i - m)
      out += d * d
    return out / l

  def confidence(self):
    return 1.96 * math.sqrt(self.variance() / len(self)) if len(self) else 0.0

  def __len__(self):
    return len(self.elements)

class BotInfo:
  def __init__(self, cmd, identity, display_name = None):
    self.cmd = cmd
    self.identity = identity
    self.display_name = display_name or cmd

  def Run(self, round_id, side):
    with open('logs/bot.stderr.id:{}.round:{}.side:{}'.format(self.identity + 1, round_id + 1, side), 'w') as stderr:
      proc = Popen(self.cmd, shell=True, stdout=PIPE, stdin=PIPE, stderr=stderr, universal_newlines=True, bufsize=1)
      return BotProc(self.identity, self.display_name, proc)

  def __cmp__(self, other):
    return cmp(self.identity, other.identity)

  def __repr__(self):
    return self.display_name


class BotProc:
  def __init__(self, identity, cmd, proc):
    self.identity = identity
    self.cmd = cmd
    self._proc = proc

  def send_init(self, bot_id, time_per_move):
    init_input = (
      'settings timebank 10000\n'
      'settings time_per_move {time_per_move}\n'
      'settings player_names player1,player2\n'
      'settings your_bot player{bot_id}\n'
      'settings your_botid {bot_id}\n'.format(bot_id=bot_id, time_per_move=time_per_move))

    self._proc.stdin.write(init_input)


  def send_update(self, round_num, field, macroboard, time_per_move):
    update_input = (
      'update game round {round}\n'
      'update game field {field}\n'
      'update game macroboard {macro}\n'
      'action move {time_per_move}\n'.format(
        round=round_num,
        field=field,
        macro=macroboard,
        time_per_move=time_per_move))

    self._proc.stdin.write(update_input)

    while True:
      line = self._proc.stdout.readline()
      if not line:
        raise RuntimeError('Process has crashed!')
      line = line.strip()
      if line:
        break

    out = line.strip().split(' ', 1)[1]
    return out

  def close(self):
    self._proc.stdin.close()
    self._proc.wait()

def RatingDelta(wins, draws, losses):
  total = wins+losses+draws
  if total == 0:
    return 0
  if wins+draws == 0 and losses>0:
    return -1000
  if losses+draws == 0 and wins>0:
    return 1000

  score = (wins + draws * 0.5) / total
  return int(round(-400 * math.log10(1.0 / score - 1.0)))

class Score:
  def __init__(self, left, right):
    self.wins = 0
    self.loses = 0
    self.draws = 0
    self.left = left
    self.right = right
    self.estimator = Estimator()
    self.estimator_nodraw = Estimator()

  def add(self, outcome):
    if outcome == 1:
      self.wins += 1
      self.estimator.add(1.0)
      self.estimator_nodraw.add(1.0)
    elif outcome == -1:
      self.loses += 1
      self.estimator.add(0.0)
      self.estimator_nodraw.add(0.0)
    elif outcome == 0:
      self.draws += 1
      self.estimator.add(0.5)
    else:
      raise ValueError('Unexpected outcome value')

  def PrintSummary(self):
    score = self.estimator.mean()
    conf = self.estimator.confidence()
    score_nodraw = self.estimator_nodraw.mean()
    conf_nodraw = self.estimator_nodraw.confidence()
    rating = RatingDelta(self.wins, self.draws, self.loses)
    total = self.wins+self.loses+self.draws

    print('Base({}):{} Test({}):{} Draws:{} Total:{} Score:{:.1f}±{:.1f}% ScoreNoDraw:{:.1f}±{:.1f}% Rating:{:+d}'.format(self.left.display_name, self.loses, self.right.display_name, self.wins, self.draws, total, score*100, conf*100, score_nodraw*100, conf_nodraw*100, rating))


class ScoreBoard:
  def __init__(self):
    self._score = {}

  def _get(self, b1, b2):
    key = (b1.identity, b2.identity)
    out = self._score.get(key, None)
    if not out:
      out = Score(b1, b2)
      self._score[key] = out
    return out

  def add(self, b1, b2, result):
    if b1.identity > b2.identity:
      b1, b2 = b2, b1
    score = self._get(b1, b2)
    if result == b1.identity:
      score.add(-1)
    elif result == b2.identity:
      score.add(1)
    elif result == -1:
      score.add(0)
    else:
      raise ValueError('Unexpected result %s' % str(result))

  def PrintSummary(self):
    print('-' * 80)
    for key in sorted(self._score):
      score = self._score[key]
      score.PrintSummary()
    sys.stdout.flush()


def ParseBots(args):
  if args.bots:
    bots = []
    identity = 0
    for cmd in args.bots:
      bots.append(BotInfo(cmd, identity))
      identity += 1

    games = []
    round_id = 0
    for _ in range((args.count+1) // 2):
      g = []
      j = len(bots) - 1
      for i in range(len(bots)-1):
        g.append((bots[i], bots[j], round_id))
        g.append((bots[j], bots[i], round_id+1))
        round_id += 2
      random.shuffle(g)
      games.extend(g)
  elif args.config:
    with open(args.config, 'r') as f:
      config = json.loads(f.read())

    bots = {}
    identity = 0
    for bot in config['bots']:
      cmd = bot['cmd']
      name = bot['name']
      info = BotInfo(cmd, identity, name)
      if name in bots:
        raise(ValueError('Duplicated bot name: {}'.format(name)))
      bots[name] = info
      identity += 1


    raw_pairs = config['pairs']
    pairs = []
    for pair in raw_pairs:
      p1 = bots[pair[0]]
      p2 = bots[pair[1]]
      pairs.append((p1, p2))

    games = []
    round_id = 0
    for _ in range((args.count+1) // 2):
      g = []
      for pair in pairs:
        g.append((pair[0], pair[1], round_id))
        g.append((pair[1], pair[0], round_id+1))
        round_id += 2
      random.shuffle(g)
      games.extend(g)
  else:
    raise(ValueError("No bots or config provided"))

  return games

def PinCPU(counter):
  with counter.get_lock():
    cpu_id = counter.value
    counter.value += 1

  pid = os.getpid()
  try:
      os.system('taskset -pc {} {}'.format(cpu_id, pid))
      print('Pinned pid {} to cpu {}'.format(pid, cpu_id))
  except:
      print('Faile to pin to cpu')

def main(args):
  games = ParseBots(args)


  hist_file = datetime.now().strftime('games-%Y%m%d-%H%M%S.txt')
  hist_path = os.path.join(args.history, hist_file)

  score_board = ScoreBoard()

  cpu_counter= multiprocessing.Value('i', lock=True)

  pool = multiprocessing.Pool(processes=args.workers, initializer=PinCPU, initargs=(cpu_counter,)) 
  with open(hist_path, 'w') as hist:
    for b1, b2, result, json in pool.imap_unordered(OneRound, games, chunksize=1):
      score_board.add(b1, b2, result)
      # print summary
      score_board.PrintSummary()
      hist.write(json)
      hist.write('\n')
      hist.flush()

class GameInfoBuilder:
  def __init__(self, bot1, bot2):
    b1 = {
        'cmd': bot1.cmd,
    }
    b2 = {
        'cmd': bot2.cmd,
    }
    self.game = {
        'bot1' : b1,
        'bot2' : b2,
    }
    self.rounds = []

  def AddRound(self, field, macro, turn, move):
    self.rounds.append({
      'turn' : turn,
      'field': field,
      'macro': macro,
      'last_move': move,
    })

  # result is 1 for bot 1, 2 for bot 2, or 0 for draw.
  def SetResult(self, result):
    self.game['result'] = result

  def JSon(self):
    self.game['rounds'] = self.rounds
    return json.dumps(self.game)

def OneRound(params):
  bot1, bot2, round_id = params

  bots = []
  try:
    game_info = GameInfoBuilder(bot1, bot2)

    positions = []

    # Get robots who are fighting (player1, player2)
    bots = [bot1.Run(round_id, 1), bot2.Run(round_id, 2)]

    # Simulate game init input
    bots[0].send_init('0', args.time_per_move)
    bots[1].send_init('1', args.time_per_move)

    # Wait for the bots to start
    time.sleep(0.5)

    round_num = 1
    move = 1
    field = ','.join(['.'] * 81)
    macroboard = ','.join(['-1'] * 9)
    turn = 0
    result = -1
    game_info.AddRound(field, macroboard, (turn + 1), '')
    if args.verbose:
      print_board(field, macroboard)
    while True:
      bot = bots[turn]
      bot_id = turn
      # Send inputs to bot
      move = bot.send_update(round_num, field, macroboard, args.time_per_move)
      # Update macroboard and game field
      field = update_field(field, move, str(bot_id))
      macroboard = update_macroboard(field, move)
      if args.verbose:
        print_board(field, macroboard)
      game_info.AddRound(field, macroboard, (turn + 1) ^ 3, move)
      # Check for winner. If winner, exit.
      if is_winner(macroboard):
        game_info.SetResult(turn+1)
        result = bot.identity
        break

      if is_draw(macroboard):
        game_info.SetResult(0)
        result = -1
        break

      round_num += 1
      turn = 1-turn
  except KeyboardInterrupt as exp:
    raise RuntimeError("Keyboard Interrupt in child")
  except Exception as exp:
    traceback.print_exc()
    logging.error(exp)
    raise
  finally:
    for b in bots:
      b.close()

  return bot1, bot2, result, game_info.JSon()

ATTRS_CODE = {
    'white': 30,
    'red': 31,
    'yellow': 33,
    'blue': 34,
    'cyan': 36,
    'bg_blue': 44,
    'bold': 1,
}

def FormatColor(text, *attrs):
  if not attrs:
    return text
  codes = ';'.join(str(ATTRS_CODE[attr]) for attr in attrs)
  out = '\033[{codes}m{text}\033[0m'.format(codes=codes,text=text)
  return out

def print_board(field, macro):
  field = field.split(',')
  macro = macro.split(',')
  
  rows = []
  while field:
    rows.append(field[:9])
    field = field[9:]

  for i, row in enumerate(rows):
    row_str = ''
    if i % 3 == 0:
      print('+' + ('---+' * 3))
    for j, cell in enumerate(row):
      if j % 3 == 0:
        row_str += '|'
      mp = (j // 3) + (i // 3) * 3
      attrs = []
      if macro[mp] == '-1':
        attrs.append('bold')
        attrs.append('bg_blue')
      if macro[mp] == '1':
        attrs.append('bold')
        attrs.append('red')
      if macro[mp] == '2':
        attrs.append('bold')
        attrs.append('blue')
      if cell == '1':
        c = 'X'
      elif cell == '2':
        c = 'O'
      else:
        c = ' '
      row_str += FormatColor(c, *attrs)
    print(row_str + '|')
  print('+' + ('---+' * 3))


def update_field(field, move, bot_id):
  col, row = move.split(' ')
  arr = field.split(',')
  index = int(row) * 9 + int(col)
  if index < 0 or index >= 81:
    raise RuntimeError('Invalid move {}.'.format(move))
  if arr[index] != '.':
    raise RuntimeError(
      'Square {col} {row} already occupied by {occ}.'.format(
        col=col, row=row, occ=arr[index]))

  arr[index] = bot_id
  return ','.join(arr)


def update_macroboard(field, move):
  # break it up into small boards
  board = field.split(',')
  small_boards = []
  for r in range(0, 9, 3):
    for c in range(0, 9, 3):
      sb = []
      sb.extend(board[r * 9 + c:r * 9 + c + 3])
      sb.extend(board[(r + 1) * 9 + c:(r + 1) * 9 + c + 3])
      sb.extend(board[(r + 2) * 9 + c:(r + 2) * 9 + c + 3])
      small_boards.append(sb)

  # determine macro board state
  def get_state(a):
    winopts = [
      [0, 1, 2],
      [3, 4, 5],
      [6, 7, 8],
      [0, 3, 6],
      [1, 4, 7],
      [2, 5, 8],
      [0, 4, 8],
      [6, 4, 2]]

    winners = ('000', '111')
    for opt in winopts:
      val = a[opt[0]] + a[opt[1]] + a[opt[2]]
      if val in winners:
        return a[opt[0]]

    return '.'

  macroboard = [get_state(b) for b in small_boards]

  # modify macro board state based on availability of small board
  col, row = move.split(' ')
  index = int(row) * 9 + int(col)
  boards = [
    [0, 3, 6, 27, 30, 33, 54, 57, 60],  # top-left
    [1, 4, 7, 28, 31, 34, 55, 58, 61],  # top-middle
    [2, 5, 8, 29, 32, 35, 56, 59, 62],  # top-right
    [9, 12, 15, 36, 39, 42, 63, 66, 69],  # middle-left
    [10, 13, 16, 37, 40, 43, 64, 67, 70],  # middle-middle
    [11, 14, 17, 38, 41, 44, 65, 68, 71],  # middle-right
    [18, 21, 24, 45, 48, 51, 72, 75, 78],  # bottom-left
    [19, 22, 25, 46, 49, 52, 73, 76, 79],  # bottom-middle
    [20, 23, 26, 47, 50, 53, 74, 77, 80]]  # bottom-right

  for i, b in enumerate(boards):
    if index in b:
      # If macro space available, update it to -1
      if macroboard[i] == '.':
        macroboard[i] = '-1'
        break
      else:  # If macro space not available, update all . to -1
        macroboard = ['-1' if m == '.' else m for m in macroboard]
        break

  return ','.join(macroboard)


def is_winner(macroboard):
  winopts = [
    [0, 1, 2],
    [3, 4, 5],
    [6, 7, 8],
    [0, 3, 6],
    [1, 4, 7],
    [2, 5, 8],
    [0, 4, 8],
    [6, 4, 2]]

  m = macroboard.split(',')
  winners = ('111', '222')
  for opt in winopts:
    val = m[opt[0]] + m[opt[1]] + m[opt[2]]
    if val in winners:
      return True

  return False

def is_draw(macroboard):
  parts = macroboard.split(',')
  if '.' in parts:
    return False
  if '-1' in parts:
    return False
  return True


if __name__ == '__main__':
  parser = argparse.ArgumentParser(description='Run two bots againts each other.')
  parser.add_argument('bots', nargs='*', help='The bots to be tested')
  parser.add_argument('--count', type=int, default=1000, help='Number of times to run the bots (Default: 1)')
  parser.add_argument('--time-per-move', type=int, default=1000, help='Milliseconds added to time bank each turn (Default: 500)')
  parser.add_argument('--workers', type=int, default=2, help='Number of parallel workers (Default: 1)')
  parser.add_argument('--history', default='history', help='Directory to store game history')
  parser.add_argument('--config', help='Config file with bots to test')
  parser.add_argument('--verbose', help='Print game progress', action='store_true')
  args = parser.parse_args()
  if args.verbose:
      print('Verbose mode enabled')
  main(args)
