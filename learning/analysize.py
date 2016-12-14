#!/usr/bin/env python3

import json
import argparse
import numpy as np
import learn

from  sklearn.linear_model import LogisticRegression
from sklearn.ensemble import GradientBoostingClassifier
from sklearn import metrics


def ParseEntry(entry):
  try:
    data = json.loads(entry)
    return data, None
  except json.decoder.JSONDecodeError as exp:
    return '', exp

def pop_count(s):
  m = {}
  for i in s:
    if i not in m:
      m[i] = 0
    m[i] += 1
  return m

def CellFeature(value):
  if value == 0:
    return 0
  if value == 1:
    return -1
  if value == 2:
    return 1

def DecodeBoard(board):
  return tuple((board >> (i * 2)) & 3 for i in range(9))

def EncodeBoard(board):
  code = 0
  for i in range(9):
    code |= (board[i] << (i * 2))
  return code

def EncodeBoards(boards):
  return [EncodeBoard(board) for board in boards]

def FlattenBoards(boards):
  out = []
  for b in boards:
    out.extend(b)
  return tuple(out)

def isDone(cells, player):
  return (
    (((cells[0]==player) and (cells[1]==player) and (cells[2]==player))) or
    (((cells[3]==player) and (cells[4]==player) and (cells[5]==player))) or
    (((cells[6]==player) and (cells[7]==player) and (cells[8]==player))) or
    (((cells[0]==player) and (cells[3]==player) and (cells[6]==player))) or
    (((cells[1]==player) and (cells[4]==player) and (cells[7]==player))) or
    (((cells[2]==player) and (cells[5]==player) and (cells[8]==player))) or
    (((cells[0]==player) and (cells[4]==player) and (cells[8]==player))) or
    (((cells[2]==player) and (cells[4]==player) and (cells[6]==player))))


def AllocArray(*dim, value=None):
  if len(dim) == 1:
    return [value] * dim[0]
  return [AllocArray(*dim[1:], value=value) for i in range(dim[0])]

micro_prob_table = AllocArray(2, 1<<18, value=None)

probs = [0.0, 0.5, 0.5, 0.0]

def ComputeWinProb(board, player):
  got = micro_prob_table[player-1][board]
  if got is not None:
    return got

  decoded = DecodeBoard(board);
  denominator = 0
  for i in range(9):
    if decoded[i] == 3:
      return 0
    if decoded[i] == 0:
      denominator += 1

  if isDone(decoded, player):
    return 1.0
  if isDone(decoded, player^3):
    return 0.0

  score = 0.0
  for i in range(9):
    for p in range(1, 3):
      if decoded[i] != 0:
        continue
      code = board | (p << (i * 2));
      score += ComputeWinProb(code, player) * 0.5 / denominator
  micro_prob_table[player-1][board] = score
  return score

def SplitMacroBoard(MacroBoard):
  boards = [[0]*9 for i in range(9)]
  for i in range(len(MacroBoard)):
    col = i % 9
    row = i // 9
    b = col // 3 + row // 3 * 3
    brow = row % 3
    bcol = col % 3
    bi = bcol + brow * 3
    boards[b][bi] = MacroBoard[i] 
  return tuple(tuple(board) for board in boards)

def CodeFormat(array, indent='', end='\n'):
  print(indent+ '{')
  if array.ndim == 1:
    print(indent + '  ', end='')
    line = 0
    for value in array:
      if line >= 4:
        print('')
        print(indent + '  ', end='')
        line = 0
      print('{: 2.8f},'.format(value), end='')
      line += 1
    print('')
  else:
    for subarray in array:
      CodeFormat(subarray, indent + '  ', end=',\n')
  print(indent + '}', end=end)

def ParseGame(game):
  if 'rounds' not in game:
    return None, None, None, 'missing rounds'
  if 'result' not in game:
    return None, None, None, 'missing result'
  outcome = game['result']
  if outcome == 0:
    result = 0.5
  elif outcome == 1:
    result = 1.0
  elif outcome == 2:
    result = 0.0

  features = []
  y_true = []
  outcomes = []
  for r in game['rounds']:
    if 'field' not in r:
      return None, None, None, 'missing field'
    field = [int(c) for c in r['field'].split(',')]
    boards = SplitMacroBoard(field)
    cell_features = tuple(CellFeature(c) for c in FlattenBoards(boards))
    features.append(cell_features)
    y_true.append(result)
    outcomes.append(outcome)
  return features, y_true, outcomes, None

def main(args):
  features = []
  y_true = []
  errors = 0
  outcomes = []
  uniques = set()
  for f in args.files:
    for line in open(f):
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

  if errors > 0:
    print('errors: {}'.format(errors))

  pop = pop_count(outcomes)
  print('size: {}'.format(len(features)))
  print('uniques: {}'.format(len(uniques)))
  print('draws: {}({:.2f}%)'.format(pop[0], pop[0]/len(features)*100))
  print('cross: {}({:.2f}%)'.format(pop[1], pop[1]/len(features)*100))
  print('circle: {}({:.2f}%)'.format(pop[2], pop[2]/len(features)*100))


  features = np.array(features, dtype='float32')
  y_true = np.array(y_true, dtype='float32')

  print(features)
  print(y_true)

  # classifier = GradientBoostingClassifier()
  classifier = learn.Classifier(len(features[0]))
  classifier.fit(features, y_true)
  print('coefs:')
  CodeFormat(classifier.coef())
  print('bias:')
  CodeFormat(classifier.bias())

  y_pred_proba = classifier.predict_proba(features)
  print(y_pred_proba)

 #  y_pred = classifier.predict(features)

 #  print(y_pred)

  print('accuracy: {:.2f}%'.format(classifier.score(features, y_true) * 100))

 #  print('Confusion matrix:')
 #  print(metrics.confusion_matrix(y_true, y_pred) / len(features))


if __name__ == '__main__':
  parser = argparse.ArgumentParser(description='Run two bots againts each other.')
  parser.add_argument('files', nargs='+', help='The files to analyse')
  args = parser.parse_args()
  main(args)
