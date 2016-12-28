#ifndef RANDOM_H
#define RANDOM_H

#include <random>
#include <limits>


extern std::default_random_engine generator;

inline int32_t RandInt32() {
  std::uniform_int_distribution<int32_t> distribution(0, std::numeric_limits<int32_t>::max());
  return distribution(generator);
}

inline int64_t RandInt64() {
  std::uniform_int_distribution<int64_t> distribution(0, std::numeric_limits<int64_t>::max());
  return distribution(generator);
}

inline int RandN(int n) {
  std::uniform_int_distribution<int> distribution(0, n-1);
  return distribution(generator);
}

inline void RandSeed(int64_t seed) {
  generator.seed(seed);
}

#endif
