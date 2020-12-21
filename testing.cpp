#include <iostream>
#include <vector>

#include "table-gen.hpp"

const auto INTERVALS = 12;
const auto OCTAVES = 3;
const auto BASE = 1 << OCTAVES;
const auto BITS = INTERVALS * OCTAVES;
const uint64_t MAX = 1UL << BITS;
const auto BUFSIZE = 4096;

typedef uint64_t Chord;

struct Condition {
	Chord mask;

	bool check_chord(Chord c) {
		return (c & mask) > 0;
	}
};

struct Rule {
	std::vector<Condition> conditions;
	std::vector<bool> truth_table;

	bool check_chord(Chord c) {
		uint64_t idx = 0;
		for (auto cnd : conditions) idx = idx << 1 | cnd.check_chord(c);
		return truth_table[idx];
	}
};

void print_chord(Chord notes) {
	for (auto octave = 0; octave < OCTAVES; ++octave) {
		for (auto interval = 0; interval < INTERVALS; ++interval) {
			auto interval_shift = (INTERVALS - interval - 1) * OCTAVES;
			if (notes >> (interval_shift + octave) & 1)
				std::cout << octave * INTERVALS + interval << ' ';
		}
	}
	std::cout << std::endl;
}

void print_bitset(Chord b) {
	for (auto i = 0; i < INTERVALS; ++i)
		std::cout << (b >> ((INTERVALS - i - 1) * OCTAVES) & (BASE-1));
	std::cout << std::endl;
}

int main() {
	Condition m6 { 0'000000007000 };
	Condition M6 { 0'000000000700 };
	std::string expr_s = "A || B";
	Expr expr(expr_s);
	Rule rule {{m6, M6}, expr.gen_table()};

	uint64_t ct = 0;
	for (uint64_t i = 0; i < MAX; ++i) {
		if (rule.check_chord(i)) ct++;
		if (i % (1UL << 32) == 0) printf("%.1f%%\n", (double) i / (double) MAX * 100.0);
	}

	std::cout << ct << " match." << std::endl;
}
