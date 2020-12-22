#ifndef CG_GENERATOR_H
#define CG_GENERATOR_H

#include <iostream>
#include <array>
#include <string>

#include "constants.hpp"
#include "helpers.hpp"

namespace generator {
	const auto BUFSIZE = 65536;
	
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
				std::cerr << ((char) (v+'A')) << ": " << (i >> (vars-v-1) & 1) << " ";
			std::cerr << "= ";
			std::cerr << table[i] << std::endl;
		}
	}

	struct Rule {
		std::vector<Condition> conditions;
		std::vector<bool> table;

		// Checks [count] chords, starting at [start] and adding [step]
		// each time. Modifies [out] and returns the number of chords
		// written.
		uint64_t check_range(uint64_t start, uint64_t count, uint64_t step, std::array<uint64_t, BUFSIZE>& out) {
			if (count > BUFSIZE) throw;

			uint64_t valid = 0;

			// Optimized loops for small numbers of conditions
			if (conditions.size() == 1) {
				for (uint64_t i = 0, j = start; i < count; ++i, j += step) {
					if (table[(j & conditions[0].mask) > 0]) out[valid++] = j;
				}
				return valid;
			} else if (conditions.size() == 2) {
				for (uint64_t i = 0, j = start; i < count; ++i, j += step) {
					if (table[((j & conditions[0].mask) > 0) * 2
						  + ((j & conditions[1].mask) > 0)]) out[valid++] = j;
				}
				return valid;
			} else if (conditions.size() == 3) {
				for (uint64_t i = 0, j = start; i < count; ++i, j += step) {
					if (table[((j & conditions[0].mask) > 0) * 4
						  + ((j & conditions[1].mask) > 0) * 2
						  + ((j & conditions[2].mask) > 0)]) out[valid++] = j;
				}
				return valid;
			}

			// General, but slow solution
			for (uint64_t i = 0, j = start; i < count; ++i, j += step) {
				auto table_idx = 0;
				for (auto cnd : conditions) table_idx = table_idx << 1 | cnd.check(j);
				if (table[table_idx]) out[valid++] = j;
			}

			return valid;
		}

		uint64_t thing(uint64_t a) { return a; }

		bool check(Chord c) {
			uint64_t idx = 0;
			for (auto cnd : conditions) idx = idx << 1 | cnd.check(c);
			return table[idx];
		}

		void print() {
			std::cerr << "Conditions:" << std::endl;
			for (auto c : conditions) {
				std::cout << '\t';
				helpers::print_bits_octal(c.mask);
			}
			std::cerr << "Truth table:" << std::endl;
			print_table(table);
		}
	};
}

#endif // CG_GENERATOR_H
