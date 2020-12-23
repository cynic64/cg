#ifndef CG_HELPERS_H
#define CG_HELPERS_H

#include <iostream>
#include <string>

namespace helpers {
	uint32_t log2(uint32_t n) {
		auto r = 0;
		while (n > 1) { r++; n >>= 1; }
		return r;
	}

	std::string fmt_vector(std::vector<int> v) {
		std::string s;
		for (auto x : v) {
			if (!s.empty()) s += " ";
			s += std::to_string(x);
		}
		return s;
	}
}
	
#endif // CF_HELPERS_H
