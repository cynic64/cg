#include <iostream>
#include <vector>
#include <variant>

const auto INTERVALS = 12;
const auto OCTAVES = 3;
const auto BASE = 1 << OCTAVES;
const auto BITS = INTERVALS * OCTAVES;
const uint64_t MAX = 1UL << BITS;
const auto BUFSIZE = 4096;

typedef uint64_t Bitset;

struct Rule {
}

struct Chord {
	Bitset notes;

	void print() {
		for (auto octave = 0; octave < OCTAVES; ++octave) {
			for (auto interval = 0; interval < INTERVALS; ++interval) {
				auto interval_shift = (INTERVALS - interval - 1) * OCTAVES;
				if (notes >> (interval_shift + octave) & 1)
					std::cout << octave * INTERVALS + interval << ' ';
			}
		}
		std::cout << std::endl;
	}
};

void print_bitset(Bitset b) {
	for (auto i = 0; i < INTERVALS; ++i)
		std::cout << (b >> ((INTERVALS - i - 1) * OCTAVES) & (BASE-1));
	std::cout << std::endl;
}

int main() {
	Rule r {0'000070000000};
	print_bitset(r.mask);

	Chord c {0'300010010000};
	print_bitset(c.notes);
	c.print();
}
