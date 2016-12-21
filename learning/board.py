def find_next_macro(macro_cells):
  count = 0
  found = -1
  for i in range(len(macro_cells)):
    cell = macro_cells[i]
    if cell == -1:
      count += 1
      found = i
      if count > 1:
        return -1
  return found

class Board():
  def __init__(self, cells, macro_cells, turn):
    self.turn = turn
    self.macro_cells = macro_cells
    self.macro_cell = find_next_macro(macro_cells)
    self.plain_cells = cells
    self.boards = SplitMacroBoard(cells)

  @classmethod
  def FromRepr(cls, cells_repr, macro_cells, turn):
    cells = [int(c) for c in cells_repr.split(',')]
    macro_cells = [int(c) for c in macro_cells.split(',')]
    return Board(cells, macro_cells, turn)

  def Rotate(self):
    return Board(RotateField(self.plain_cells), RotateField(self.macro_cells, 3), self.turn)

  def Flatten(self):
    return FlattenBoards(self.boards)

  def __getitem__(self, i):
    return self.boards[i]

  def Clone(self):
    return Board(self.cells, self.macro_cells, self.turn)

  def Tick(self, macro, micro, player):
    self.boards[macro][micro] = player
    pc = (macro % 3) * 3 + micro % 3
    pr = (macro // 3) * 3 + micro // 3
    self.plain_cells[pc + pc*9] = player


def DecodeBoard(board):
  return [(board >> (i * 2)) & 3 for i in range(9)]

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
  return out

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
  return boards

def RotateField(field, size=9):
  out = [0] * (size*size)
  for i in range(size*size):
    row = i // size
    col = i % size
    nrow = col
    ncol = (size-1) - row
    out[ncol + nrow*size] = field[i]
  return out

def BoardRotations(board):
  yield board
  for i in range(3):
    board = board.Rotate()
    yield board
