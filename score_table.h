#ifndef SCORE_TABLE_H
#define SCORE_TABLE_H

extern double macro_score_table[1<<18];

extern double micro_win_prob[1<<18];
extern double micro_lose_prob[1<<18];

void InitScoreTable();

extern int reg_cell_score_int[9*9];
extern int reg_turn_coef_int;
extern int reg_cell_bias_int;

static const int score_factor = 1000000;

#endif
