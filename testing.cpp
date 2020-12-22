#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "constants.hpp"
#include "parse.hpp"

int main(int argc, char *argv[]) {
	if (argc < 2) {
		std::cout << "Not enough args" << std::endl;
		throw;
	}

	std::string user_rule = "0: ";
	user_rule += argv[1];

	std::ifstream infile("rules.txt");
	if (!infile.is_open()) throw;

	parse::Ruleset rules;
	parse::read_rules_from_file(infile, rules);

	if (!parse::read_rule(user_rule, rules)) {
		std::cout << "User rule references unspecified rules!" << std::endl;
		throw;
	}

	auto rule = rules["0"];
	rule.print();
	uint64_t count = 0;
	for (uint64_t i = 0, j = 0; i < MAX; ++i, j += 33554393) {
		if (i % (1UL << 32) == 0) std::cout << ((double) i / (double) MAX * 100.0) << "%" << std::endl;
		if (rule.check(j)) {
			generator::print_chord(j);
			count++;
		}
	}

	std::cout << std::endl << count << "total." << std::endl;
}
