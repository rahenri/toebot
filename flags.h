#ifndef FLAGS_H
#define FLAGS_H

#define _DEFAULT
// #define _DETERMINISTIC

// Dsiable it for now, it doens't seem to be better.
static const bool EnablePonder = false;

#ifdef _LOCAL

// Disable opening table when running locally.
static const bool EnableOpeningTable = false;

#ifdef _DETERMINISTIC
static const bool PlayDeterministic = true;
static const bool PrintSearchTree = false;
static const bool AnalysisMode = false;
static const int DefaultTimeLimit = 1000000000;
static const int MaxDepth = 12;
static const int MinTimeLimit = 0;
#endif

#ifdef _ANALYSIS
static const bool PlayDeterministic = false;
static const bool PrintSearchTree = false;
static const bool AnalysisMode = true;
static const int DefaultTimeLimit = 800;
static const int MaxDepth = 50;
static const int MinTimeLimit = 300000;
#endif

#ifdef _DEFAULT
static const bool PlayDeterministic = false;
static const bool PrintSearchTree = false;
static const bool AnalysisMode = false;
static const int DefaultTimeLimit = 800;
static const int MaxDepth = 50;
static const int MinTimeLimit = 0;
#endif


#else
static const bool PlayDeterministic = false;
static const bool PrintSearchTree = false;
static const bool AnalysisMode = false;
static const int DefaultTimeLimit = 800;
static const int MaxDepth = 50;
static const int MinTimeLimit = 0;
static const bool EnableOpeningTable = true;
#endif

#endif

