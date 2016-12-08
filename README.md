# Toebot

Toebot is an AI for palying ultimate tic tac toe game in the theaigames.com competition.

# Progress

- v27 Candidate

I decided to explore pondering a little more, this is what I get comparing pondering vs pondering with effectively one cpu per game:

Base(./toebot):180 Test(./toebot --ponder):409 Draws:411 Total:1000 Score:61.5±2.3% Rating:+81

Which is a notable improvement.

Next I need to run the same experiment but with two cpus per game.
So far it looks like the bot perform worse


- v26 Candidate

Base(./toebot-v25):195 Test(./toebot-v26):684 Draws:1121 Total:2000 Ratio:77.82±2.75%

Base(./toebot-v24):263 Test(./toebot-v26):1212 Draws:525 Total:2000 Ratio:82.17±1.95%

Base(./toebot-v23):542 Test(./toebot-v26):753 Draws:705 Total:2000 Ratio:58.15±2.69%

Experiment with shortening threshold:

```
Base(./toebot-v23):236 Test(./toebot-v26):692 Draws:72 Total:1000 Ratio:74.57±2.80%
Base(./toebot-v23):153 Test(./toebot-v26 --shortening-threshold 30000 --depth-shortening 4):734 Draws:113 Total:1000 Ratio:82.75±2.49%
Base(./toebot-v23):107 Test(./toebot-v26 --shortening-threshold 40000 --depth-shortening 4):639 Draws:254 Total:1000 Ratio:85.66±2.52%
Base(./toebot-v26):154 Test(./toebot-v26 --shortening-threshold 30000 --depth-shortening 4):322 Draws:524 Total:1000 Ratio:67.65±4.20%
Base(./toebot-v26):6 Test(./toebot-v26 --shortening-threshold 40000 --depth-shortening 4):440 Draws:554 Total:1000 Ratio:98.65±1.07%
Base(./toebot-v26 --shortening-threshold 30000 --depth-shortening 4):159 Test(./toebot-v26 --shortening-threshold 40000 --depth-shortening 4):381 Draws:460 Total:1000 Ratio:70.56±3.84%

```

Experiment with depth shortening:

```
Base(./toebot-v23):34 Test(./toebot-v26):147 Draws:51 Total:232 Ratio:81.22±5.69%
Base(./toebot-v23):20 Test(./toebot-v26 --depth-shortening 6):175 Draws:37 Total:232 Ratio:89.74±4.26%
Base(./toebot-v23):20 Test(./toebot-v26 --depth-shortening 8):170 Draws:42 Total:232 Ratio:89.47±4.36%
Base(./toebot-v26):58 Test(./toebot-v26 --depth-shortening 6):43 Draws:130 Total:231 Ratio:42.57±9.64%
Base(./toebot-v26):98 Test(./toebot-v26 --depth-shortening 8):39 Draws:94 Total:231 Ratio:28.47±7.56%
Base(./toebot-v26 --depth-shortening 6):44 Test(./toebot-v26 --depth-shortening 8):46 Draws:142 Total:232 Ratio:51.11±10.33%
```

Experiment with shortening threshold no 2:

```
Base(./toebot-v23):105 Test(./toebot-v26 --shortening-threshold 40000):663 Draws:232 Total:1000 Ratio:86.33±2.43%
Base(./toebot-v23):59 Test(./toebot-v26 --shortening-threshold 50000):807 Draws:134 Total:1000 Ratio:93.19±1.68%
Base(./toebot-v23):43 Test(./toebot-v26 --shortening-threshold 60000):859 Draws:98 Total:1000 Ratio:95.23±1.39%
Base(./toebot-v26 --shortening-threshold 40000):65 Test(./toebot-v26 --shortening-threshold 50000):237 Draws:698 Total:1000 Ratio:78.48±4.64%
Base(./toebot-v26 --shortening-threshold 40000):217 Test(./toebot-v26 --shortening-threshold 60000):149 Draws:634 Total:1000 Ratio:40.71±5.03%
Base(./toebot-v26 --shortening-threshold 50000):201 Test(./toebot-v26 --shortening-threshold 60000):79 Draws:720 Total:1000 Ratio:28.21±5.27%
```

Partial result:

```
Base(./toebot-v23):43 Test(./toebot-v24):97 Draws:97 Total:237 Ratio:69.29±7.64%
Base(./toebot-v23):36 Test(./toebot-v25):161 Draws:39 Total:236 Ratio:81.73±5.40%
Base(./toebot-v23):11 Test(./toebot-v26):188 Draws:38 Total:237 Ratio:94.47±3.18%
Base(./toebot-v24):91 Test(./toebot-v25):115 Draws:30 Total:236 Ratio:55.83±6.78%
Base(./toebot-v24):35 Test(./toebot-v26):131 Draws:70 Total:236 Ratio:78.92±6.21%
Base(./toebot-v25):26 Test(./toebot-v26):109 Draws:101 Total:236 Ratio:80.74±6.65%
```

A partial experiment comparing v26 with v25, strangely, v26 with depth-shortening=0 should be equivalent to v25, but they seem to be fairly different:

```
Base(./toebot-v25):78 Test(./toebot-v26 --depth-shortening 0):40 Draws:129 Total:247 Ratio:33.90±8.54%
Base(./toebot-v25):15 Test(./toebot-v26 --shortening-threshold 20000 --depth-shortening 4):94 Draws:138 Total:247 Ratio:86.24±6.47%
Base(./toebot-v25):1 Test(./toebot-v26 --shortening-threshold 40000 --depth-shortening 4):126 Draws:120 Total:247 Ratio:99.21±1.54%
Base(./toebot-v25):13 Test(./toebot-v26 --shortening-threshold 50000 --depth-shortening 4):113 Draws:122 Total:248 Ratio:89.68±5.31%
Base(./toebot-v26 --depth-shortening 0):21 Test(./toebot-v26 --shortening-threshold 20000 --depth-shortening 4):87 Draws:139 Total:247 Ratio:80.56±7.46%
Base(./toebot-v26 --depth-shortening 0):1 Test(./toebot-v26 --shortening-threshold 40000 --depth-shortening 4):101 Draws:145 Total:247 Ratio:99.02±1.91%
Base(./toebot-v26 --depth-shortening 0):2 Test(./toebot-v26 --shortening-threshold 50000 --depth-shortening 4):124 Draws:122 Total:248 Ratio:98.41±2.18%
Base(./toebot-v26 --shortening-threshold 20000 --depth-shortening 4):54 Test(./toebot-v26 --shortening-threshold 40000 --depth-shortening 4):80 Draws:113 Total:247 Ratio:59.70±8.31%
Base(./toebot-v26 --shortening-threshold 20000 --depth-shortening 4):62 Test(./toebot-v26 --shortening-threshold 50000 --depth-shortening 4):91 Draws:93 Total:246 Ratio:59.48±7.78%
Base(./toebot-v26 --shortening-threshold 40000 --depth-shortening 4):19 Test(./toebot-v26 --shortening-threshold 50000 --depth-shortening 4):65 Draws:163 Total:247 Ratio:77.38±8.95%
```

Final experiment:

```
Base(./toebot-v23):199 Test(./toebot-v24):413 Draws:388 Total:1000 Ratio:67.48±3.71%
Base(./toebot-v23):171 Test(./toebot-v25):641 Draws:188 Total:1000 Ratio:78.94±2.80%
Base(./toebot-v23):60 Test(./toebot-v26):799 Draws:141 Total:1000 Ratio:93.02±1.70%
Base(./toebot-v24):368 Test(./toebot-v25):486 Draws:146 Total:1000 Ratio:56.91±3.32%
Base(./toebot-v24):185 Test(./toebot-v26):558 Draws:257 Total:1000 Ratio:75.10±3.11%
Base(./toebot-v25):45 Test(./toebot-v26):471 Draws:484 Total:1000 Ratio:91.28±2.43%
```

From this experiment, v26 should have +158 more elo points than v25.

Time experiment:

```
Base(./toebot-v26 --default-time-limit 100):174 Test(./toebot-v26 --default-time-limit 250):379 Draws:351 Total:904 Score:61.3±2.4% Rating:+80
Base(./toebot-v26 --default-time-limit 100):85 Test(./toebot-v26 --default-time-limit 500):341 Draws:476 Total:902 Score:64.2±2.0% Rating:+101
Base(./toebot-v26 --default-time-limit 100):45 Test(./toebot-v26 --default-time-limit 800):497 Draws:361 Total:903 Score:75.0±1.9% Rating:+191
Base(./toebot-v26 --default-time-limit 250):311 Test(./toebot-v26 --default-time-limit 500):234 Draws:359 Total:904 Score:45.7±2.5% Rating:-30
Base(./toebot-v26 --default-time-limit 250):82 Test(./toebot-v26 --default-time-limit 800):412 Draws:408 Total:902 Score:68.3±2.1% Rating:+133
Base(./toebot-v26 --default-time-limit 500):71 Test(./toebot-v26 --default-time-limit 800):158 Draws:675 Total:904 Score:54.8±1.6% Rating:+34
```

Suprprisingly, the one with 500ms was beaten by the one with 250ms.

- v25

Base(./toebot-v24):676 Test(./toebot-v25):972 Draws:352 Total:2000 Ratio:58.98±2.37%

Base(./toebot-v23):376 Test(./toebot-v25):1255 Draws:369 Total:2000 Ratio:76.95±2.04%

- v24

Base(./toebot-v23):377 Test(./toebot-v24):834 Draws:789 Total:2000 Ratio:68.87±2.61%
