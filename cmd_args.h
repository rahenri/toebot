#ifndef CMD_ARGS_HH
#define CMD_ARGS_HH

#include <string>

const int* NewIntFlag(const std::string& name, int def);
const std::string* NewStringFlag(const std::string& name, const std::string& def);
const bool* NewBoolFlag(const std::string& name, bool def);

bool ParseFlags(int argc, const char** argv);

#endif
