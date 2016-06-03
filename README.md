# Toebot

Toebot is an AI for palying ultimate tic tac toe game in the theaigames.com competition.

# Progress

- v26 Candidate

Base(./toebot-v25):195 Test(./toebot-v26):684 Draws:1121 Total:2000 Ratio:77.82±2.75%

Base(./toebot-v24):263 Test(./toebot-v26):1212 Draws:525 Total:2000 Ratio:82.17±1.95%

Base(./toebot-v23):542 Test(./toebot-v26):753 Draws:705 Total:2000 Ratio:58.15±2.69%


Experimen with shortening threshold:
Base(./toebot-v23):236 Test(./toebot-v26):692 Draws:72 Total:1000 Ratio:74.57±2.80%
Base(./toebot-v23):153 Test(./toebot-v26 --shortening-threshold 30000 --depth-shortening 4):734 Draws:113 Total:1000 Ratio:82.75±2.49%
Base(./toebot-v23):107 Test(./toebot-v26 --shortening-threshold 40000 --depth-shortening 4):639 Draws:254 Total:1000 Ratio:85.66±2.52%
Base(./toebot-v26):154 Test(./toebot-v26 --shortening-threshold 30000 --depth-shortening 4):322 Draws:524 Total:1000 Ratio:67.65±4.20%
Base(./toebot-v26):6 Test(./toebot-v26 --shortening-threshold 40000 --depth-shortening 4):440 Draws:554 Total:1000 Ratio:98.65±1.07%
Base(./toebot-v26 --shortening-threshold 30000 --depth-shortening 4):159 Test(./toebot-v26 --shortening-threshold 40000 --depth-shortening 4):381 Draws:460 Total:1000 Ratio:70.56±3.84%


- v25

Base(./toebot-v24):676 Test(./toebot-v25):972 Draws:352 Total:2000 Ratio:58.98±2.37%

Base(./toebot-v23):376 Test(./toebot-v25):1255 Draws:369 Total:2000 Ratio:76.95±2.04%

- v24

Base(./toebot-v23):377 Test(./toebot-v24):834 Draws:789 Total:2000 Ratio:68.87±2.61%
