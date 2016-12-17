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

def RotateField(field):
  out = [0] * (9*9)
  for i in range(9*9):
    row = i // 9
    col = i % 9
    nrow = col
    ncol = 8 - row
    out[ncol + nrow*9] = field[i]
  return tuple(out)

def FieldRotations(field):
  yield field
  for i in range(3):
    field = RotateField(field)
    yield field

def ReorgField(field):
  return FlattenBoards(SplitMacroBoard(field))
