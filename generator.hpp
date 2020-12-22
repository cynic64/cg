#ifndef CG_GENERATOR_H
#define CG_GENERATOR_H

#include <iostream>
#include <vector>
#include <string>
#include <chrono>

#include "chord.hpp"
#include "helpers.hpp"

namespace generator {
	const auto BUFSIZE = 65536;
	const std::chrono::duration<float> TIMEOUT(0.25);
	
	void print_table(std::vector<bool> table) {
		auto vars = helpers::log2(table.size());

		for (size_t i = 0; i < table.size(); ++i) {
			for (uint32_t v = 0; v < vars; ++v)
				std::cerr << ((char) (v+'A')) << ": " << (i >> (vars-v-1) & 1) << " ";
			std::cerr << "= ";
			std::cerr << table[i] << std::endl;
		}
	}

	struct Rule {
		std::vector<uint32_t> conditions;
		std::vector<bool> table;

		// Checks chords starting at [start] and outputting to [out],
		// which should already by resized to BUFSIZE. Returns the
		// number of chords checked and stopping position when either
		// out is filled or stop is reached or more than TIMEOUT seconds
		// pass.
		std::pair<uint32_t, uint32_t> check_range(uint32_t start, uint32_t stop, std::vector<uint32_t>& out) {
			if (out.size() != BUFSIZE) throw;

			auto start_time = std::chrono::high_resolution_clock::now();

			uint32_t valid = 0;
			uint32_t i = start;

			// Optimized loops for small numbers of conditions
			if (conditions.size() == 1) {
				for (; i < stop && valid < BUFSIZE; ++i) {
					if (table[(i & conditions[0]) > 0]) out[valid++] = i;
					if (i % 32768 == 0 && std::chrono::high_resolution_clock::now() - start_time > TIMEOUT) break;
				}
				return {valid, i};
			} else if (conditions.size() == 2) {
				for (; i < stop && valid < BUFSIZE; ++i) {
					if (table[((i & conditions[0]) > 0) * 2
						  + ((i & conditions[1]) > 0)]) out[valid++] = i;
					if (i % 32768 == 0 && std::chrono::high_resolution_clock::now() - start_time > TIMEOUT) break;
				}
				return {valid, i};
			} else if (conditions.size() == 3) {
				for (; i < stop && valid < BUFSIZE; ++i) {
					if (table[((i & conditions[0]) > 0) * 4
						  + ((i & conditions[1]) > 0) * 2
						  + ((i & conditions[2]) > 0)]) out[valid++] = i;
					if (i % 32768 == 0 && std::chrono::high_resolution_clock::now() - start_time > TIMEOUT) break;
				}
				return {valid, i};
			}

			// General, but slow solution
			for (i = start; i < stop && valid < BUFSIZE; ++i) {
				auto table_idx = 0;
				for (auto cnd : conditions) table_idx = table_idx << 1 | ((i & cnd) > 0);
				if (table[table_idx]) out[valid++] = i;
				if (i % 32768 == 0 && std::chrono::high_resolution_clock::now() - start_time > TIMEOUT) break;
			}

			return {valid, i};
		}

		void print() {
			std::cerr << "Conditions:" << std::endl;
			for (auto c : conditions) {
				std::cout << '\t';
				chord::print_mixed(c);
			}
			std::cerr << "Truth table:" << std::endl;
			print_table(table);
		}
	};
}

#endif // CG_GENERATOR_H
