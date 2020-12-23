#ifndef CG_CHORD_H
#define CG_CHORD_H

#include <cstdint>
#include <array>
#include <string>
#include <iostream>
#include <algorithm>
#include <vector>
#include <sstream>

#include "helpers.hpp"

namespace chord {
        // Octaves (and therefore bits) per interval:
        // 0 1 2 3 4 5 6 7 8 9 10 11
        // 3 3 3 3 3 3 3 3 2 2  2  2
	// Interval 11 goes in the ones place
	typedef uint32_t IntervalBits;

	const auto INTERVALS = 12;
	const std::array<int, INTERVALS> BITS_PER_INTERVAL = {3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2};
	const auto BITS = 32;
	const uint32_t MAX = UINT32_MAX;

	// C6, 20th fret E
	const int HIGHEST_NOTE = 72;
	// E0
	const int LOWEST_NOTE = -8;

	struct Chord {
		// Root is an offset from C1
		Chord(IntervalBits c, int root) : root(root) {
			for (auto octave = 0; octave < *std::max_element(BITS_PER_INTERVAL.begin(), BITS_PER_INTERVAL.end()); ++octave) {
				auto interval_shift = BITS;
				for (auto interval = 0; interval < INTERVALS; ++interval) {
					if (BITS_PER_INTERVAL[interval] <= octave) break;
					interval_shift -= BITS_PER_INTERVAL[interval];
					if (c >> (interval_shift + octave) & 1)
						notes.push_back(octave * INTERVALS + interval);
				}
			}
		}

		Chord(std::string& s) {
			std::istringstream iss(s);
			std::string token;
			iss >> token;
			root = stoi(token.substr(1));

			while (iss >> token) notes.push_back(stoi(token));
		}

		void transpose(int shift) {
			root += shift;
		}

		bool within_range() {
			if (notes.size() == 0) return true;
			return (root + notes[0]) >= LOWEST_NOTE && (root + notes.back()) <= HIGHEST_NOTE;
		}

		std::string fmt() {
			std::string s;
			s += "(";
			s += std::to_string(root);
			s += ") ";
			s += helpers::fmt_vector(notes);
			return s;
		}

		// Offset from C1
		int root;
		// Offsets from root
		std::vector<int> notes;
	};

	IntervalBits bits_from_string(std::string& in) {
		if (in.size() != INTERVALS) throw;

		IntervalBits out = 0;
		for (auto i = 0; i < INTERVALS; ++i) {
			auto bits = BITS_PER_INTERVAL[i];
			out <<= bits;
			auto n = in[i] - '0';
			out |= n;
		}

		return out;
	}

	std::string fmt_binary(IntervalBits c) {
		std::string s;
		auto i = BITS - 1;
		for (auto bits : BITS_PER_INTERVAL) {
			for (auto j = 0; j < bits; ++j, i--) s += std::to_string((c >> i) & 1);
			s += " ";
		}
		return s;
	}

	std::string fmt_mixed(IntervalBits c) {
		std::string s;
		auto shift = BITS;
		for (auto bits : BITS_PER_INTERVAL) {
			shift -= bits;
			auto n = (c >> shift) & ((1 << bits) - 1);
			s += std::to_string(n);
		}
		return s;
	}
}

#endif // CG_CHORD_H
