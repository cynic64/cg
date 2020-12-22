#ifndef CG_CHORD_H
#define CG_CHORD_H

#include <cstdint>
#include <array>
#include <string>
#include <iostream>
#include <algorithm>

namespace chord {
        // Octaves (and therefore bits) per interval:
        // 0 1 2 3 4 5 6 7 8 9 10 11
        // 3 3 3 3 3 3 3 3 2 2  2  2
	// Interval 11 goes in the ones place
	typedef uint32_t Chord;

	const auto INTERVALS = 12;
	const std::array<int, INTERVALS> BITS_PER_INTERVAL = {3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2};
	const auto BITS = 32;
	const uint32_t MAX = UINT32_MAX;

	Chord from_string(std::string& in) {
		if (in.size() != INTERVALS) throw;

		Chord out = 0;
		for (auto i = 0; i < INTERVALS; ++i) {
			auto bits = BITS_PER_INTERVAL[i];
			out <<= bits;
			auto n = in[i] - '0';
			out |= n;
		}

		return out;
	}

	void print_intervals(Chord c) {
		for (auto octave = 0; octave < *std::max_element(BITS_PER_INTERVAL.begin(), BITS_PER_INTERVAL.end()); ++octave) {
			auto interval_shift = BITS;
			for (auto interval = 0; interval < INTERVALS; ++interval) {
				if (BITS_PER_INTERVAL[interval] <= octave) break;
				interval_shift -= BITS_PER_INTERVAL[interval];
				if (c >> (interval_shift + octave) & 1)
					std::cout << octave * INTERVALS + interval << " ";
			}
		}
		std::cout << std::endl;
	}

	void print_binary(Chord c) {
		auto i = BITS - 1;
		for (auto bits : BITS_PER_INTERVAL) {
			for (auto j = 0; j < bits; ++j, i--) std::cout << ((c >> i) & 1);
			std::cout << " ";
		}
		std::cout << std::endl;
	}

	void print_mixed(Chord c) {
		auto shift = BITS;
		for (auto bits : BITS_PER_INTERVAL) {
			shift -= bits;
			auto n = (c >> shift) & ((1 << bits) - 1);
			std::cout << n;
		}
		std::cout << std::endl;
	}
}

#endif // CG_CHORD_H
