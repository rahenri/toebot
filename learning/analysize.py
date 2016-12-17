#!/usr/bin/env python3

import argparse
import numpy as np
import learn

# from  sklearn.linear_model import LogisticRegression
# from sklearn.ensemble import GradientBoostingClassifier
from sklearn import metrics

def pop_count(s):
  m = {}
  for i in s:
    if i not in m:
      m[i] = 0
    m[i] += 1
  return m

def CodeFormat(array, indent='', end=';\n', output=None):
  print(indent+ '{', file=output)
  if array.ndim == 1:
    print(indent + '  ', end='', file=output)
    line = 0
    for value in array:
      if line >= 4:
        print('', file=output)
        print(indent + '  ', end='', file=output)
        line = 0
      print('{: 2.8f},'.format(value), end='', file=output)
      line += 1
    print('', file=output)
  else:
    for subarray in array:
      CodeFormat(subarray, indent + '  ', end=',\n')
  print(indent + '}', end=end, file=output)

def GenCode(output, coefs, bias):
  board_coefs = coefs[:81]
  heuristic_coef = coefs[81]
  turn_coef = coefs[82]
  board_coefs /= heuristic_coef
  bias /= heuristic_coef
  output.write('double cell_score[81] = ')
  CodeFormat(board_coefs, output=output)
  output.write('\n')
  output.write('double turn_coef = {: 2.8f};\n'.format(turn_coef))
  output.write('\n')
  output.write('double reg_cell_bias = {: 2.8f};\n'.format(bias[0]))
  output.write('\n')

def main(args):
  features = []
  y_true = []
  outcomes = []

  for f in args.files:
    data = np.load(f)
    if len(data['features']) == 0:
      continue
    features.append(data['features'])
    y_true.append(data['y_true'])
    outcomes.append(data['outcomes'])

  features = np.concatenate(features)
  y_true = np.concatenate(y_true)
  outcomes = np.concatenate(outcomes)

  pop = pop_count(outcomes)
  print('size: {}'.format(len(features)))
  print('draws: {}({:.2f}%)'.format(pop.get(0, 0), pop.get(0, 0)/len(features)*100))
  print('cross: {}({:.2f}%)'.format(pop.get(1, 0), pop.get(1, 0)/len(features)*100))
  print('circle: {}({:.2f}%)'.format(pop.get(2, 0), pop.get(2, 0)/len(features)*100))


  features = np.array(features, dtype='float32')
  y_true = np.array(y_true, dtype='float32')

  # classifier = GradientBoostingClassifier()
  # classifier = LogisticRegression()
  classifier = learn.Classifier(len(features[0][0]), len(features[0]))
  classifier.fit(features, y_true)
  print('coefs:')
  CodeFormat(classifier.coef_)
  print('bias:')
  CodeFormat(classifier.intercept_)

  y_pred_proba = classifier.predict_proba(features)
  print('proba:')
  print(y_pred_proba)

  print('score: {:.2f}%'.format(classifier.score(features, y_true) * 100))

  y_pred_cat = np.round(y_pred_proba * 2).astype('int')
  y_true_cat = np.round(y_true * 2).astype('int')
  correct = (y_pred_cat == y_true_cat)
  print('accuracy: {:.2f}%'.format(np.mean(correct) * 100))

  print('Confusion matrix:')
  print(metrics.confusion_matrix(y_true_cat, y_pred_cat))

  with open('model.cpp', 'w') as f:
    GenCode(f, classifier.coef_, classifier.intercept_)

  # skclassifier = LogisticRegression(C=1000.0)
  # skfeatures = features.reshape([-1, len(features[0][0])])
  # sky_true = (y_true_cat.reshape([-1, 1]) + np.array([0, 0, 0, 0])).reshape([-1])
  # skclassifier.fit(skfeatures, sky_true)
  # print('skscore: {:.2f}%'.format(skclassifier.score(skfeatures, sky_true)*100))
  # sky_pred = skclassifier.predict(skfeatures)
  # print('SK Confusion matrix:')
  # print(metrics.confusion_matrix(sky_true, sky_pred))

if __name__ == '__main__':
  parser = argparse.ArgumentParser(description='Run two bots againts each other.')
  parser.add_argument('files', nargs='+', help='The files to analyse')
  args = parser.parse_args()
  main(args)
