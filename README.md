# Toebot

Toebot is an AI for palying ultimate tic tac toe game in the theaigames.com competition.

# Progress

- v31

Experimenting with pondering and sorting:

```
Base(v30):86 Test(sorting):74 Draws:130 Total:290 Score:47.9±4.3% ScoreNoDraw:46.2±7.7% Rating:-14
Base(v30):74 Test(pondering):104 Draws:114 Total:292 Score:55.1±4.4% ScoreNoDraw:58.4±7.2% Rating:+36
Base(v30):121 Test(pondering-sorting):102 Draws:68 Total:291 Score:46.7±5.0% ScoreNoDraw:45.7±6.5% Rating:-23
```

Experimenting with conservative hash:

```
Base(toebot-aggressive):252 Test(toebot-pondering-sorting-conservative):496 Draws:252 Total:1000 Score:62.2±2.6% ScoreNoDraw:66.3±3.4% Rating:+87
```




- v27 Candidate

I decided to explore pondering a little more, this is what I get comparing pondering vs pondering with effectively one cpu per game:

Base(./toebot):180 Test(./toebot --ponder):409 Draws:411 Total:1000 Score:61.5±2.3% Rating:+81

Which is a notable improvement.

Next I need to run the same experiment but with two cpus per game.
So far it looks like the bot perform worse


Now, moving on to use some RL, here I add one feature for each cell board on
top of the current heuristic as a single features. And perform a linear
combination of all features. I trained the weights through RL and obtained the
following results after about 2 days of training:

```
Base(./toebot-v24):77 Test(./toebot-candidate):576 Draws:347 Total:1000 Score:75.0±2.0% Rating:+190
Base(./toebot-v25):165 Test(./toebot-candidate):613 Draws:222 Total:1000 Score:72.4±2.4% Rating:+168
Base(./toebot-v26):192 Test(./toebot-candidate):309 Draws:499 Total:1000 Score:55.9±2.2% Rating:+41
```

it was a small improvement over v26, but it doesn't perform as well against
v25. maybe v26 was exploiting some of v25 week points. I should tune the depth
shortening heuristic a bit after this.

```
Base(./toebot-v27):370 Test(./toebot-v27 --shortening-threshold 4000):198 Draws:432 Total:1000 Score:41.4±2.3% Rating:-60
Base(./toebot-v27):504 Test(./toebot-v27 --shortening-threshold 3000):348 Draws:148 Total:1000 Score:42.2±2.8% Rating:-55
Base(./toebot-v27):406 Test(./toebot-v27 --shortening-threshold 6000):184 Draws:410 Total:1000 Score:38.9±2.3% Rating:-78
Base(./toebot-v27):83 Test(./toebot-v27 --shortening-threshold 7000):175 Draws:742 Total:1000 Score:54.6±1.5% Rating:+32
```

```
Base(./toebot-v27):15 Test(./toebot-v27 --shortening-threshold 8000):14 Draws:44 Total:73 Score:49.3±7.2% ScoreNoDraw:48.3±0.2% Rating:-5
Base(./toebot-v27):35 Test(./toebot-v27 --shortening-threshold 9000):19 Draws:19 Total:73 Score:39.0±9.5% ScoreNoDraw:35.2±0.1% Rating:-77
Base(./toebot-v26):34 Test(./toebot-v27 --shortening-threshold 7000):16 Draws:22 Total:72 Score:37.5±9.2% ScoreNoDraw:32.0±0.1% Rating:-89
Base(./toebot-v25):21 Test(./toebot-v27 --shortening-threshold 7000):31 Draws:22 Total:74 Score:56.8±9.4% ScoreNoDraw:59.6±0.1% Rating:+47
Base(./toebot-v24):14 Test(./toebot-v27 --shortening-threshold 7000):49 Draws:10 Total:73 Score:74.0±9.1% ScoreNoDraw:77.8±0.1% Rating:+181
```


```
Base(v26):98 Test(v27-40000):194 Draws:216 Total:508 Score:59.4±3.2% ScoreNoDraw:66.4±5.4% Rating:+66
Base(v26):89 Test(v27-30000):168 Draws:251 Total:508 Score:57.8±3.0% ScoreNoDraw:65.4±5.8% Rating:+54
Base(v26):109 Test(v27-50000):152 Draws:247 Total:508 Score:54.2±3.1% ScoreNoDraw:58.2±6.0% Rating:+29
Base(v25):119 Test(v27-40000):205 Draws:184 Total:508 Score:58.5±3.4% ScoreNoDraw:63.3±5.2% Rating:+59
Base(v25):134 Test(v27-30000):180 Draws:194 Total:508 Score:54.5±3.4% ScoreNoDraw:57.3±5.5% Rating:+32
Base(v25):83 Test(v27-50000):293 Draws:132 Total:508 Score:70.7±3.3% ScoreNoDraw:77.9±4.2% Rating:+153
Base(v24):60 Test(v27-40000):353 Draws:95 Total:508 Score:78.8±3.0% ScoreNoDraw:85.5±3.4% Rating:+228
Base(v24):56 Test(v27-30000):347 Draws:105 Total:508 Score:78.6±3.0% ScoreNoDraw:86.1±3.4% Rating:+226
Base(v24):30 Test(v27-50000):292 Draws:185 Total:507 Score:75.8±2.6% ScoreNoDraw:90.7±3.2% Rating:+199
```

Looks like v27 is the winner.



- v26 candidate

base(./toebot-v25):195 test(./toebot-v26):684 draws:1121 total:2000 ratio:77.82±2.75%

base(./toebot-v24):263 test(./toebot-v26):1212 draws:525 total:2000 ratio:82.17±1.95%

base(./toebot-v23):542 test(./toebot-v26):753 draws:705 total:2000 ratio:58.15±2.69%

experiment with shortening threshold:

```
base(./toebot-v23):236 test(./toebot-v26):692 draws:72 total:1000 ratio:74.57±2.80%
base(./toebot-v23):153 test(./toebot-v26 --shortening-threshold 30000 --depth-shortening 4):734 draws:113 total:1000 ratio:82.75±2.49%
base(./toebot-v23):107 test(./toebot-v26 --shortening-threshold 40000 --depth-shortening 4):639 draws:254 total:1000 ratio:85.66±2.52%
base(./toebot-v26):154 test(./toebot-v26 --shortening-threshold 30000 --depth-shortening 4):322 draws:524 total:1000 ratio:67.65±4.20%
base(./toebot-v26):6 test(./toebot-v26 --shortening-threshold 40000 --depth-shortening 4):440 draws:554 total:1000 ratio:98.65±1.07%
base(./toebot-v26 --shortening-threshold 30000 --depth-shortening 4):159 test(./toebot-v26 --shortening-threshold 40000 --depth-shortening 4):381 draws:460 total:1000 ratio:70.56±3.84%

```

experiment with depth shortening:

```
base(./toebot-v23):34 test(./toebot-v26):147 draws:51 total:232 ratio:81.22±5.69%
base(./toebot-v23):20 test(./toebot-v26 --depth-shortening 6):175 draws:37 total:232 ratio:89.74±4.26%
base(./toebot-v23):20 test(./toebot-v26 --depth-shortening 8):170 draws:42 total:232 ratio:89.47±4.36%
base(./toebot-v26):58 test(./toebot-v26 --depth-shortening 6):43 draws:130 total:231 ratio:42.57±9.64%
base(./toebot-v26):98 test(./toebot-v26 --depth-shortening 8):39 draws:94 total:231 ratio:28.47±7.56%
base(./toebot-v26 --depth-shortening 6):44 test(./toebot-v26 --depth-shortening 8):46 draws:142 total:232 ratio:51.11±10.33%
```

experiment with shortening threshold no 2:

```
base(./toebot-v23):105 test(./toebot-v26 --shortening-threshold 40000):663 draws:232 total:1000 ratio:86.33±2.43%
base(./toebot-v23):59 test(./toebot-v26 --shortening-threshold 50000):807 draws:134 total:1000 ratio:93.19±1.68%
base(./toebot-v23):43 test(./toebot-v26 --shortening-threshold 60000):859 draws:98 total:1000 ratio:95.23±1.39%
base(./toebot-v26 --shortening-threshold 40000):65 test(./toebot-v26 --shortening-threshold 50000):237 draws:698 total:1000 ratio:78.48±4.64%
base(./toebot-v26 --shortening-threshold 40000):217 test(./toebot-v26 --shortening-threshold 60000):149 draws:634 total:1000 ratio:40.71±5.03%
base(./toebot-v26 --shortening-threshold 50000):201 test(./toebot-v26 --shortening-threshold 60000):79 draws:720 total:1000 ratio:28.21±5.27%
```

partial result:

```
base(./toebot-v23):43 test(./toebot-v24):97 draws:97 total:237 ratio:69.29±7.64%
base(./toebot-v23):36 test(./toebot-v25):161 draws:39 total:236 ratio:81.73±5.40%
base(./toebot-v23):11 test(./toebot-v26):188 draws:38 total:237 ratio:94.47±3.18%
base(./toebot-v24):91 test(./toebot-v25):115 draws:30 total:236 ratio:55.83±6.78%
base(./toebot-v24):35 test(./toebot-v26):131 draws:70 total:236 ratio:78.92±6.21%
base(./toebot-v25):26 test(./toebot-v26):109 draws:101 total:236 ratio:80.74±6.65%
```

a partial experiment comparing v26 with v25, strangely, v26 with depth-shortening=0 should be equivalent to v25, but they seem to be fairly different:

```
base(./toebot-v25):78 test(./toebot-v26 --depth-shortening 0):40 draws:129 total:247 ratio:33.90±8.54%
base(./toebot-v25):15 test(./toebot-v26 --shortening-threshold 20000 --depth-shortening 4):94 draws:138 total:247 ratio:86.24±6.47%
base(./toebot-v25):1 test(./toebot-v26 --shortening-threshold 40000 --depth-shortening 4):126 draws:120 total:247 ratio:99.21±1.54%
base(./toebot-v25):13 test(./toebot-v26 --shortening-threshold 50000 --depth-shortening 4):113 draws:122 total:248 ratio:89.68±5.31%
base(./toebot-v26 --depth-shortening 0):21 test(./toebot-v26 --shortening-threshold 20000 --depth-shortening 4):87 draws:139 total:247 ratio:80.56±7.46%
base(./toebot-v26 --depth-shortening 0):1 test(./toebot-v26 --shortening-threshold 40000 --depth-shortening 4):101 draws:145 total:247 ratio:99.02±1.91%
base(./toebot-v26 --depth-shortening 0):2 test(./toebot-v26 --shortening-threshold 50000 --depth-shortening 4):124 draws:122 total:248 ratio:98.41±2.18%
base(./toebot-v26 --shortening-threshold 20000 --depth-shortening 4):54 test(./toebot-v26 --shortening-threshold 40000 --depth-shortening 4):80 draws:113 total:247 ratio:59.70±8.31%
base(./toebot-v26 --shortening-threshold 20000 --depth-shortening 4):62 test(./toebot-v26 --shortening-threshold 50000 --depth-shortening 4):91 draws:93 total:246 ratio:59.48±7.78%
base(./toebot-v26 --shortening-threshold 40000 --depth-shortening 4):19 test(./toebot-v26 --shortening-threshold 50000 --depth-shortening 4):65 draws:163 total:247 ratio:77.38±8.95%
```

final experiment:

```
base(./toebot-v23):199 test(./toebot-v24):413 draws:388 total:1000 ratio:67.48±3.71%
base(./toebot-v23):171 test(./toebot-v25):641 draws:188 total:1000 ratio:78.94±2.80%
base(./toebot-v23):60 test(./toebot-v26):799 draws:141 total:1000 ratio:93.02±1.70%
base(./toebot-v24):368 test(./toebot-v25):486 draws:146 total:1000 ratio:56.91±3.32%
base(./toebot-v24):185 test(./toebot-v26):558 draws:257 total:1000 ratio:75.10±3.11%
base(./toebot-v25):45 test(./toebot-v26):471 draws:484 total:1000 ratio:91.28±2.43%
```

from this experiment, v26 should have +158 more elo points than v25.

time experiment:

```
base(./toebot-v26 --default-time-limit 100):174 test(./toebot-v26 --default-time-limit 250):379 draws:351 total:904 score:61.3±2.4% rating:+80
base(./toebot-v26 --default-time-limit 100):85 test(./toebot-v26 --default-time-limit 500):341 draws:476 total:902 score:64.2±2.0% rating:+101
base(./toebot-v26 --default-time-limit 100):45 test(./toebot-v26 --default-time-limit 800):497 draws:361 total:903 score:75.0±1.9% rating:+191
base(./toebot-v26 --default-time-limit 250):311 test(./toebot-v26 --default-time-limit 500):234 draws:359 total:904 score:45.7±2.5% rating:-30
base(./toebot-v26 --default-time-limit 250):82 test(./toebot-v26 --default-time-limit 800):412 draws:408 total:902 score:68.3±2.1% rating:+133
base(./toebot-v26 --default-time-limit 500):71 test(./toebot-v26 --default-time-limit 800):158 draws:675 total:904 score:54.8±1.6% rating:+34
```

suprprisingly, the one with 500ms was beaten by the one with 250ms.

- v25

base(./toebot-v24):676 test(./toebot-v25):972 draws:352 total:2000 ratio:58.98±2.37%

base(./toebot-v23):376 test(./toebot-v25):1255 draws:369 total:2000 ratio:76.95±2.04%

- v24

base(./toebot-v23):377 test(./toebot-v24):834 draws:789 total:2000 ratio:68.87±2.61%
