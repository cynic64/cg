#ifndef CG_CONSTANTS_H
#define CG_CONSTANTS_H

const auto INTERVALS = 12;
const auto OCTAVES = 3;
const auto BASE = 1 << OCTAVES;
const auto BITS = INTERVALS * OCTAVES;
const uint64_t MAX = 1UL << BITS;
const auto BUFSIZE = 4096;

#endif // CG_CONSTANTS_H
