#!/usr/bin/python

import os
import sys
from subprocess import Popen, PIPE, STDOUT


def main(argv):
    # Get robots who are fighting (player1, player2)
    bot1, bot2 = get_bots(argv[1], argv[2])
    # Simulate game init input
    send_init('1', bot1)
    send_init('2', bot2)
    round_num = 1
    move = 1
    field = ','.join(['0'] * 81)
    macroboard = ','.join(['-1'] * 9)
    print_board(field, macroboard, round_num, '')
    while True:
        for bot_id, bot in [('1', bot1), ('2', bot2)]:
            # Send inputs to bot
            move = send_update(bot, round_num, move, field, macroboard)
            # Update macroboard and game field
            field = update_field(field, move, str(bot_id))
            macroboard = update_macroboard(field, move)
            # Check for winner. If winner, exit.
            print_board(field, macroboard, round_num, move)
            if is_winner(macroboard):
                return

            round_num += 1


def get_bots(path1, path2):
    bot1 = Popen([path1],
                 stdout=PIPE,
                 stdin=PIPE)
    bot2 = Popen([path2],
                 stdout=PIPE,
                 stdin=PIPE)

    return bot1, bot2


def send_init(bot_id, bot):
    init_input = (
        'settings timebank 10000\n'
        'settings time_per_move 500\n'
        'settings player_names player1,player2\n'
        'settings your_bot player{bot_id}\n'
        'settings your_botid {bot_id}\n'.format(bot_id=bot_id))

    bot.stdin.write(init_input)


def send_update(bot, round_num, move, field, macroboard):
    update_input = (
        'update game round {round}\n'
        'update game move {move}\n'
        'update game field {field}\n'
        'update game macroboard {macro}\n'
        'action move 10000\n'.format(
            round=round_num,
            move=move,
            field=field,
            macro=macroboard))

    bot.stdin.write(update_input)
    out = bot.stdout.readline().strip()
    print 'bot output: ' + repr(out)
    return out


def update_field(field, move, bot_id):
    print move
    col, row = move.split(' ')[1:3]
    arr = field.split(',')
    index = int(row) * 9 + int(col)
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


def print_board(field, macroboard, round_num, move):
    field = field.replace('0', ' ')
    a = field.split(',')
    msg = ''
    for i in range(0, 81, 9):
        if not i % 27 and i > 0:
            msg += '---+---+---\n'

        msg += '|'.join([
            ''.join(a[i:i+3]),
            ''.join(a[i+3:i+6]),
            ''.join(a[i+6:i+9])]) + '\n'

    msg += '\nRound {}\nmacroboard: {}\nfield: {}\nmove: {}\n'.format(
        round_num, macroboard, field, move)

    sys.stdout.write(msg)


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
            print 'WINNER! Player {}'.format(m[opt[0]])
            return True

    return False


if __name__ == '__main__':
    main(sys.argv)
