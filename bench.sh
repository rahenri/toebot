#!/bin/bash

time ./toebot --max-depth 12 --min-time-limit 100000000 "$@" <<< self_play
