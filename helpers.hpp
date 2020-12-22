#ifndef CG_HELPERS_H
#define CG_HELPERS_H

#include <iostream>

#include "constants.hpp"

namespace helpers {
	void print_bits_octal(uint64_t b, bool newline = true) {
		for (auto i = 0; i < INTERVALS; ++i)
			std::cerr << (b >> ((INTERVALS - i - 1) * OCTAVES) & (BASE-1));
		if (newline) std::cerr << std::endl;
	}

	uint64_t log2(uint64_t n) {
		auto r = 0;
		while (n > 1) { r++; n >>= 1; }
		return r;
	}

	void print_chord(Chord c) {
		for (auto octave = 0; octave < OCTAVES; ++octave) {
			for (auto interval = 0; interval < INTERVALS; ++interval) {
				auto interval_shift = (INTERVALS - interval - 1) * OCTAVES;
				if (c >> (interval_shift + octave) & 1)
					std::cout << octave * INTERVALS + interval << ' ';
			}
		}
		std::cout << std::endl;
	}
}
	
#endif // CF_HELPERS_H
