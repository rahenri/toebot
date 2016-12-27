#!/bin/sh

exec ./third_party/test.py --config "eval.cfg" --count 1000 | tee output-eval
