#!/usr/bin/env python3

import numpy as np
import argparse
import json
import score

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

  features = []
  y_true = []
  outcomes = []
  for r in game['rounds']:
    if 'field' not in r:
      return None, None, None, 'missing field'
    field = [int(c) for c in r['field'].split(',')]
    cell_features = score.GenFeatures(field)
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
  uniques = set()
  for line in FileLineReader(*args.files):
    game, err = ParseEntry(line)
    if err != None:
      errors += 1
      continue
    feats, y, outs, err = ParseGame(game)
    if err is not None:
      errors += 1
      continue
    y_true.extend(y)
    features.extend(feats)
    outcomes.extend(outs)
    for i in range(len(feats)):
      key = (feats[i], outs[i])
      uniques.add(key)

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
