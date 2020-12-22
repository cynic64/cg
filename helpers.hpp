#ifndef CG_HELPERS_H
#define CG_HELPERS_H

#include <iostream>

#include "constants.hpp"

namespace helpers {
	void print_bits_octal(uint64_t b, bool newline = true) {
		for (auto i = 0; i < INTERVALS; ++i)
			std::cout << (b >> ((INTERVALS - i - 1) * OCTAVES) & (BASE-1));
		if (newline) std::cout << std::endl;
	}

	uint64_t log2(uint64_t n) {
		auto r = 0;
		while (n > 1) { r++; n >>= 1; }
		return r;
	}
}
	
#endif // CF_HELPERS_H
