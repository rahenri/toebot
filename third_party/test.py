#!/usr/bin/python
# coding=utf-8

from __future__ import division

import os
import sys
import argparse
import random
import multiprocessing
import logging
import time
import math

from subprocess import Popen, PIPE, STDOUT

bot_stderr = open('stderr', 'w')

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
  def __init__(self, cmd, identity):
    self.cmd = cmd
    self.identity = identity

  def Run(self, round_id):
    with open('logs/bot.stderr.{}.{}'.format(self.identity + 1, round_id + 1), 'w') as stderr:
      return BotProc(self.identity, self.cmd, Popen(self.cmd, shell=True, stdout=PIPE, stdin=PIPE, stderr=stderr))

  def __cmp__(self, other):
    return cmp(self.identity, other.identity)


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
    out = self._proc.stdout.readline().strip()
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

  def add(self, outcome):
    if outcome == 1:
      self.wins += 1
      self.estimator.add(1.0)
    elif outcome == -1:
      self.loses += 1
      self.estimator.add(0.0)
    elif outcome == 0:
      self.draws += 1
      self.estimator.add(0.5)

  def PrintSummary(self):
    score = self.estimator.mean()
    conf = self.estimator.confidence()
    rating = RatingDelta(self.wins, self.draws, self.loses)
    total = self.wins+self.loses+self.draws

    print 'Base({}):{} Test({}):{} Draws:{} Total:{} Score:{:.1f}±{:.1f}% Rating:{:+d}'.format(self.left.cmd, self.loses, self.right.cmd, self.wins, self.draws, total, score*100, conf*100, rating)


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
    print '-' * 80
    for key in sorted(self._score):
      score = self._score[key]
      score.PrintSummary()
    sys.stdout.flush()


def main(args):
  bots = []
  identity = 0
  for cmd in args.bots:
    bots.append(BotInfo(cmd, identity))
    identity += 1

  score_board = ScoreBoard()

  games = []
  round_id = 0
  for _ in range((args.count+1) // 2):
    g = []
    for i in range(len(bots)):
      for j in range(i+1, len(bots)):
        g.append((bots[i], bots[j], round_id))
        g.append((bots[j], bots[i], round_id+1))
        round_id += 2
    random.shuffle(g)
    games.extend(g)

  pool = multiprocessing.Pool(processes=args.workers) 

  for b1, b2, result in pool.imap(OneRound, games, chunksize=1):
    score_board.add(b1, b2, result)

    # print summary
    score_board.PrintSummary()


def OneRound((bot1, bot2, round_id)):
  try:
    # Get robots who are fighting (player1, player2)
    bots = [bot1.Run(round_id), bot2.Run(round_id)]

    # Simulate game init input
    bots[0].send_init('1', args.time_per_move)
    bots[1].send_init('2', args.time_per_move)

    # Wait two second for the bots to start
    time.sleep(2)

    round_num = 1
    move = 1
    field = ','.join(['0'] * 81)
    macroboard = ','.join(['-1'] * 9)
    turn = 0
    result = -1
    while True:
      bot = bots[turn]
      bot_id = turn+1
      # Send inputs to bot
      move = bot.send_update(round_num, field, macroboard, args.time_per_move)
      # Update macroboard and game field
      field = update_field(field, move, str(bot_id))
      macroboard = update_macroboard(field, move)
      # Check for winner. If winner, exit.
      if is_winner(macroboard):
        result = bot.identity
        bot_stderr.write('Bot %d(%s) won\n' % (bot.identity, bot.cmd))
        bot_stderr.flush()
        break

      if is_draw(macroboard):
        result = -1
        break

      round_num += 1
      turn = 1-turn
  except KeyboardInterrupt as exp:
    raise RuntimeError("Keyboard Interrupt in child")
  except Exception as exp:
    logging.error(exp)
    raise
  finally:
    for b in bots:
      b.close()

  return bot1, bot2, result


def update_field(field, move, bot_id):
  col, row = move.split(' ')[1:3]
  arr = field.split(',')
  index = int(row) * 9 + int(col)
  if index < 0 or index >= 81:
    raise RuntimeError('Invalid move {}.'.format(move))
  if arr[index] != '0':
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

    winners = ('111', '222')
    for opt in winopts:
      val = a[opt[0]] + a[opt[1]] + a[opt[2]]
      if val in winners:
        return a[opt[0]]

    if '0' not in a:
      return '3'

    return '0'

  macroboard = [get_state(b) for b in small_boards]

  # modify macro board state based on availability of small board
  col, row = move.split(' ')[1:3]
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
      if macroboard[i] == '0':
        macroboard[i] = '-1'
        break
      else:  # If macro space not available, update all 0 to -1
        macroboard = ['-1' if m == '0' else m for m in macroboard]
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
  if '0' in parts:
    return False
  if '-1' in parts:
    return False
  return True


if __name__ == '__main__':
  parser = argparse.ArgumentParser(description='Run two bots againts each other.')
  parser.add_argument('bots', nargs='+', help='The bots to be tested')
  parser.add_argument('--count', type=int, default=1000, help='Number of times to run the bots (Default: 1)')
  parser.add_argument('--time-per-move', type=int, default=1000, help='Milliseconds added to time bank each turn (Default: 500)')
  parser.add_argument('--workers', type=int, default=2, help='Number of parallel workers (Default: 1)')
  args = parser.parse_args()
  main(args)
