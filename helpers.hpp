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

	std::string fmt_vector(std::vector<std::string>& v) {
		std::string s;
		for (auto x : v) {
			if (!s.empty()) s += " ";
			s += x;
		}
		return s;
	}

	std::string fmt_vector(std::vector<int> v) {
		std::vector<std::string> converted;
		for (auto x : v) converted.push_back(std::to_string(x));
		return fmt_vector(converted);
	}
}
	
#endif // CF_HELPERS_H
