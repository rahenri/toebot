#ifndef FLAGS_H
#define FLAGS_H

#ifdef _LOCAL

static const bool PlayDeterministic = false;
static const bool PrintSearchTree = false;
static const bool PonderMode = false;
static const int DefaultTimeLimit = 800;
static const int MaxDepth = 50;

#else

static const bool PlayDeterministic = false;
static const bool PrintSearchTree = false;
static const bool PonderMode = false;
static const int DefaultTimeLimit = 800;
static const int MaxDepth = 50;

#endif

#endif
