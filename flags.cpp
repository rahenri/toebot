#include "flags.h"
#include "cmd_args.h"

// Disable it for now, it doens't seem to be better.
const bool* EnablePonder = NewBoolFlag("enable-pondering", true);

const int* DepthShortening = NewIntFlag("depth-shortening", 4);

const int* ShorteningThreshold = NewIntFlag("shortening-threshold", 50000);

const int* MaxDepth = NewIntFlag("max-depth", 100);

const bool* AnalysisMode = NewBoolFlag("analysis", false);

// Disable opening table for now.
const bool* EnableOpeningTable = NewBoolFlag("enable-opening-table", false);

const int* MinTimeLimit = NewIntFlag("min-time-limit", 0);
const int* DefaultTimeLimit = NewIntFlag("default-time-limit", 800);

const int *HashTableSize = NewIntFlag("hash-size", 50000017);
