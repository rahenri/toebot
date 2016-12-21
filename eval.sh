#!/bin/sh

exec ./third_party/test.py ./toebot-v24 ./toebot-v25 ./toebot-v26 ./toebot-candidate --count 1000 | tee output-eval
