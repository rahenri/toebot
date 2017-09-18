#!/bin/sh

cp toebot toebot-candidate

exec ./third_party/test.py --config "eval.cfg" --count 1000 "$@" | tee output-eval
