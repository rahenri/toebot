#ifndef FLAGS_H
#define FLAGS_H

#define _DEFAULT
// #define _DETERMINISTIC

// Disable it for now, it doens't seem to be better.
static const bool EnablePonder = false;

// Disable opening table for now.
static const bool EnableOpeningTable = false;

#ifdef _LOCAL

#ifdef _DETERMINISTIC
static const bool PrintSearchTree = false;
static const bool AnalysisMode = false;
static const int DefaultTimeLimit = 1000000000;
static const int MaxDepth = 12;
static const int MinTimeLimit = 0;
#endif

#ifdef _ANALYSIS
static const bool PrintSearchTree = false;
static const bool AnalysisMode = true;
static const int DefaultTimeLimit = 800;
static const int MaxDepth = 50;
static const int MinTimeLimit = 300000;
#endif

#ifdef _DEFAULT
static const bool PrintSearchTree = false;
static const bool AnalysisMode = false;
static const int DefaultTimeLimit = 800;
static const int MaxDepth = 50;
static const int MinTimeLimit = 0;
#endif


#else
static const bool PrintSearchTree = false;
static const bool AnalysisMode = false;
static const int DefaultTimeLimit = 800;
static const int MaxDepth = 50;
static const int MinTimeLimit = 0;
#endif

#endif

