#ifndef SCORE_TABLE_H
#define SCORE_TABLE_H


struct ScoreCell {
  double score = -2;
};

extern ScoreCell score_lookup_table[1<<18];

void InitScoreTable();


#endif
