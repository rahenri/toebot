#ifndef SCORE_TABLE_H
#define SCORE_TABLE_H

extern double macro_score_table[1<<18];

extern double micro_win_prob[1<<18];
extern double micro_lose_prob[1<<18];

void InitScoreTable();

#endif
