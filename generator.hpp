#ifndef CG_GENERATOR_H
#define CG_GENERATOR_H

#include <iostream>
#include <vector>
#include <string>

namespace generator {
	const auto INTERVALS = 12;
	const auto OCTAVES = 3;
	const auto BASE = 1 << OCTAVES;
	const auto BITS = INTERVALS * OCTAVES;
	const uint64_t MAX = 1UL << BITS;
	const auto BUFSIZE = 4096;

	typedef uint64_t Chord;

	struct Condition {
		uint64_t mask;

		Condition(std::string& token) {
			mask = std::stoul(token, nullptr, BASE);
		}

		bool check(Chord c) {
			return (c & mask) > 0;
		}
	};

	void print_bits_octal(uint64_t b, bool newline = true) {
		for (auto i = 0; i < INTERVALS; ++i)
			std::cout << (b >> ((INTERVALS - i - 1) * OCTAVES) & (BASE-1));
		if (newline) std::cout << std::endl;
	}

	struct Rule {
		std::vector<Condition> conditions;
		std::vector<bool> truth_table;

		bool check(Chord c) {
			uint64_t idx = 0;
			for (auto cnd : conditions) idx = idx << 1 | cnd.check(c);
			return truth_table[idx];
		}

		void print() {
			std::cout << "Conditions:" << std::endl;
			for (auto c : conditions) {
				std::cout << '\t';
				print_bits_octal(c.mask);
			}
			std::cout << "Truth table has size " << truth_table.size() << std::endl;
		}
	};

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

#endif // CG_GENERATOR_H
