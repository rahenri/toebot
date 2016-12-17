import board

def CellFeature(value):
  if value == 0:
    return 0
  if value == 1:
    return -1
  if value == 2:
    return 1

def AllocArray(*dim, value=None):
  if len(dim) == 1:
    return [value] * dim[0]
  return [AllocArray(*dim[1:], value=value) for i in range(dim[0])]

micro_prob_table = AllocArray(2, 1<<18, value=None)

probs = (0.0, 0.5, 0.5, 0.0)

def ComputeWinProb(b, player):
  got = micro_prob_table[player-1][b]
  if got is not None:
    return got

  decoded = board.DecodeBoard(b);
  denominator = 0
  for i in range(9):
    if decoded[i] == 3:
      return 0
    if decoded[i] == 0:
      denominator += 1

  if board.isDone(decoded, player):
    return 1.0
  if board.isDone(decoded, player^3):
    return 0.0

  score = 0.0
  for i in range(9):
    if decoded[i] != 0:
      continue
    for p in range(1, 3):
      code = b | (p << (i * 2));
      score += ComputeWinProb(code, player) * 0.5 / denominator
  micro_prob_table[player-1][b] = score
  return score

WIN_LINES = (
  (0, 1, 2),
  (3, 4, 5),
  (6, 7, 8),
  (0, 3, 6),
  (1, 4, 7),
  (2, 5, 8),
  (0, 4, 8),
  (2, 4, 6),
)

def HeuristicFeature(b):
  sum = 0.0
  for line in WIN_LINES:
    p1 = 1.0
    p2 = 1.0
    for p in line:
      p1 *= ComputeWinProb(board.EncodeBoard(b[p]), 1)
      p2 *= ComputeWinProb(board.EncodeBoard(b[p]), 2)
    sum += p1 - p2
  return sum

def BoardFeatures(b):
    out = [CellFeature(c) for c in board.FlattenBoards(b)]
    out.append(HeuristicFeature(b))
    return tuple(out)

def GenFeatures(field):
  count = 0
  for c in field:
    if c != 0:
      count += 1
  turn_feature = (count % 2) * 2 - 1
  out = []
  for field in board.FieldRotations(field):
    b = board.SplitMacroBoard(field)
    out.append(BoardFeatures(b))
    out.append(turn)
  return tuple(sorted(out))

def run_tests():
  weights = (
   0.10840570, 0.11869533, 0.08203787, 0.10907566,
   0.11313321, 0.07956099, 0.08471585, 0.07195589,
   0.09687658, 0.08979735, 0.07071941, 0.08984441,
   0.05068656, 0.14116749, 0.07683529, 0.12839910,
   0.06885862, 0.09610151, 0.08471582, 0.10907566,
   0.10840569, 0.07195589, 0.11313321, 0.11869532,
   0.09687657, 0.07956100, 0.08203790, 0.08984444,
   0.07683532, 0.09610151, 0.07071938, 0.14116752,
   0.06885864, 0.08979737, 0.05068659, 0.12839912,
   0.12983812, 0.08349726, 0.12983812, 0.08349726,
  -0.03586163, 0.08349726, 0.12983811, 0.08349726,
   0.12983809, 0.12839912, 0.05068658, 0.08979736,
   0.06885864, 0.14116745, 0.07071940, 0.09610150,
   0.07683532, 0.08984444, 0.08203787, 0.07956100,
   0.09687655, 0.11869534, 0.11313321, 0.07195589,
   0.10840570, 0.10907569, 0.08471583, 0.09610149,
   0.06885865, 0.12839912, 0.07683530, 0.14116746,
   0.05068658, 0.08984444, 0.07071941, 0.08979738,
   0.09687658, 0.07195589, 0.08471585, 0.07956100,
   0.11313322, 0.10907569, 0.08203788, 0.11869538,
   0.10840571, 1.0)
  bias = -0.19994880;

  field = "1,0,0,2,0,0,0,1,0,1,0,0,2,0,1,0,0,0,0,2,1,2,2,0,0,0,0,0,1,2,2,0,0,0,1,0,1,0,2,0,1,0,2,0,2,0,1,0,0,0,0,0,1,0,1,0,0,2,1,0,0,2,0,0,0,0,2,0,1,0,0,0,0,0,0,0,0,0,0,0,0"
  field = tuple(int(c) for c in field.split(','))
  b = board.SplitMacroBoard(field)
  f = HeuristicFeature(b)
  print(f)

  feats = GenFeatures(field)
  s = bias
  for i in range(len(feats[0])):
    s += feats[0][i] * weights[i]

  print(int(s * 1000000))

if __name__ == '__main__':
  run_tests()
