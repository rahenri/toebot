#include "cmd_args.h"

#include <cerrno>
#include <string>
#include <iostream>

using namespace std;

struct AbstractFlag {
  AbstractFlag(const string& name) : name(name) {}
  const string name;
  virtual bool Parse(const string& value) = 0;
};

template <class T>
struct BaseFlag : public AbstractFlag {
  BaseFlag(const string& name, const T& def) : AbstractFlag(name), value(def) {}
  T value;

  const T* Ptr() {
    return &this->value;
  }

  virtual bool Parse(const string& value) = 0;
};

struct IntFlag : public BaseFlag<int> {
  IntFlag(const string& name, int def) : BaseFlag(name, def) {}
  virtual bool Parse(const string& value) {
    errno = 0;
    char* end = nullptr;
    const char* str = value.c_str();
    int size = value.size();
    this->value = strtol(str, &end, 10);
    return (errno == 0 && end == str + size);
  }
};

struct StringFlag : public BaseFlag<string> {
  StringFlag(const string& name, const string& def) : BaseFlag(name, def) {}
  virtual bool Parse(const string& value) {
    this->value = value;
    return true;
  }
};

struct BoolFlag : public BaseFlag<bool> {
  BoolFlag(const string& name, bool def) : BaseFlag(name, def) {}
  virtual bool Parse(const string& value) {
    if (value == "" || value == "true") {
      this->value = true;
    } else if (value == "false") {
      this->value = false;
    } else {
      return false;
    }
    return true;
  }
};

static AbstractFlag* all_flags[30];
static int num_flags = 0;

const int* NewIntFlag(const string& name, int def) {
  auto flag = new IntFlag(name, def);
  all_flags[num_flags++] = flag;
  return flag->Ptr();
}

const string* NewStringFlag(const string& name, const string& def) {
  auto flag = new StringFlag(name, def);
  all_flags[num_flags++] = flag;
  return flag->Ptr();
}

const bool* NewBoolFlag(const string& name, const bool def) {
  auto flag = new BoolFlag(name, def);
  all_flags[num_flags++] = flag;
  return flag->Ptr();
}

AbstractFlag* FindFlag(const string& name) {
  for (int i = 0; all_flags[i] != nullptr; i++) {
    if (all_flags[i]->name == name) {
      return all_flags[i];
    }
  }
  return nullptr;
}

bool isDoubleDash(const string& v) {
  if (v.size() < 2) {
    return false;
  }
  return v[0] == '-' && v[1] == '-';
}

bool isSingleDash(const string& v) {
  if (v.empty()) {
    return false;
  }
  if (v[0] != '-') {
    return false;
  }
  return !isDoubleDash(v);
}

bool isArg(const string& v) {
  return isDoubleDash(v) || isSingleDash(v);
}

string StripDash(const string& v) {
  if (isSingleDash(v)) {
    return v.substr(1);
  }
  if (isDoubleDash(v)) {
    return v.substr(2);
  }
  return v;
}

bool HasValue(const string& v) {
  for (const char c : v) {
    if (c == '=') {
      return true;
    }
  }
  return false;
}

string GetValue(const string& v) {
  for (int i = 0; i < int(v.size()); i++) {
    if (v[i] == '=') {
      return v.substr(i+1);
    }
  }
  return "";
}

string StripValue(const string& v) {
  for (int i = 0; i < int(v.size()); i++) {
    if (v[i] == '=') {
      return v.substr(0, i);
    }
  }
  return "";
}

bool ParseFlags(int argc, const char** argv) {
  string v;
  for (int i = 1; i < argc; i++) {
    v = argv[i];
    if (!isArg(v)) {
      cerr << "Loose argument: " << v << endl;
      return false;
    }
    v = StripDash(v);
    auto flag = FindFlag(v);
    if (flag == nullptr) {
      cerr << "Unknown flag: --" << v << endl;
    }
    string value;
    if (HasValue(v)) {
      value = GetValue(v);
      v = StripValue(v);
    } else {
      if (i+1 == argc || isArg(argv[i+1])) {
        value = "";
      } else {
        i++;
        value = argv[i];
      }
    }
    if (!flag->Parse(value)) {
      cerr << "Failed to parse flag --" << v << endl;
    }
  }

  return true;
}
