#!/bin/sh

while true; do
  ./third_party/test.py ./toebot-v24 ./toebot-v25 ./toebot-v26 ./toebot --count 32 --time-per-move 100
  make -j4 data
  ./learning/analysize.py history/*npz
  make;
done
