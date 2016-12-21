#!/usr/bin/env python3

import numpy as np
import argparse
import json
import score
import board

def ParseEntry(entry):
  try:
    data = json.loads(entry)
    return data, None
  except json.decoder.JSONDecodeError as exp:
    return '', exp

def FileLineReader(*files):
  for filename in files:
    with open(filename) as f:
      for line in f:
        yield line

def ParseGame(game):
  if 'rounds' not in game:
    return None, None, None, 'missing rounds'
  if 'result' not in game:
    return None, None, None, 'missing result'
  outcome = game['result']
  if outcome == 0:
    result = 0.5
  elif outcome == 1:
    result = 1
  elif outcome == 2:
    result = 0
  else:
    return None, None, None, 'invalid outcome'

  cmd1 = game['bot1']['cmd']
  cmd2 = game['bot2']['cmd']
  if cmd1 == './toebot':
    our_player = 1
  elif cmd2 == './toebot':
    our_player = 2
  else:
    return None, None, None, 'Our player not found'

  features = []
  y_true = []
  outcomes = []
  for r in game['rounds']:
    turn = r['turn']
    if turn  == our_player:
      # if it is the opponent's turn, it means *we* took the action that led to
      # the current board position, so we want to train on that only.
      continue
    if 'field' not in r:
      return None, None, None, 'missing field'
    b = board.Board.FromRepr(r['field'], r['macro'], r['turn'])
    cell_features = score.GenFeatures(b)
    features.append(cell_features)
    y_true.append(result)
    outcomes.append(outcome)
  return features, y_true, outcomes, None

def FileLineReader(*files):
  for filename in files:
    with open(filename) as f:
      for line in f:
        yield line

def main(args):
  features = []
  y_true = []
  errors = 0
  outcomes = []
  for line in FileLineReader(*args.files):
    game, err = ParseEntry(line)
    if err != None:
      errors += 1
      continue
    feats, y, outs, err = ParseGame(game)
    if err is not None:
      print(err)
      errors += 1
      continue
    y_true.extend(y)
    features.extend(feats)
    outcomes.extend(outs)
    for i in range(len(feats)):
      key = (feats[i], outs[i])

  features = np.array(features, dtype='float32')
  y_true = np.array(y_true, dtype='float32')
  outcomes = np.array(outcomes)

  if errors > 0:
    print('errors: {}'.format(errors))

  np.savez(args.output, features=features, y_true=y_true, outcomes=outcomes)

if __name__ == '__main__':
  parser = argparse.ArgumentParser(description='Compile json data into features in numpy format.')
  parser.add_argument('files', nargs='+', help='The files to analyse')
  parser.add_argument('--output', help='Output npz file')
  args = parser.parse_args()
  main(args)
