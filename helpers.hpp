#ifndef CG_HELPERS_H
#define CG_HELPERS_H

#include <iostream>

namespace helpers {
	uint32_t log2(uint32_t n) {
		auto r = 0;
		while (n > 1) { r++; n >>= 1; }
		return r;
	}
}
	
#endif // CF_HELPERS_H
