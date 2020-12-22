#ifndef CG_GENERATOR_H
#define CG_GENERATOR_H

#include <iostream>
#include <vector>
#include <string>

#include "constants.hpp"
#include "helpers.hpp"

namespace generator {
	typedef uint64_t Chord;

	struct Condition {
		uint64_t mask;

		bool check(Chord c) {
			return (c & mask) > 0;
		}
	};

	void print_table(std::vector<bool> table) {
		auto vars = helpers::log2(table.size());

		for (uint64_t i = 0; i < table.size(); ++i) {
			for (uint64_t v = 0; v < vars; ++v)
				std::cout << ((char) (v+'A')) << ": " << (i >> (vars-v-1) & 1) << " ";
			std::cout << "= ";
			std::cout << table[i] << std::endl;
		}
	}

	struct Rule {
		std::vector<Condition> conditions;
		std::vector<bool> table;

		bool check(Chord c) {
			uint64_t idx = 0;
			for (auto cnd : conditions) idx = idx << 1 | cnd.check(c);
			return table[idx];
		}

		void print() {
			std::cout << "Conditions:" << std::endl;
			for (auto c : conditions) {
				std::cout << '\t';
				helpers::print_bits_octal(c.mask);
			}
			std::cout << "Truth table:" << std::endl;
			print_table(table);
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
