#include <iostream>
#include <fstream>
#include <string>

#include "expression.hpp"
#include "parse.hpp"

int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("Not enough args\n");
		exit(1);
	}

	std::string user_rule = argv[1];

	std::ifstream infile ("rules.txt");
	if (!infile.is_open()) throw;
	std::string rule_text;
	for (std::string line; std::getline(infile, line);) { rule_text += line; rule_text += "\n"; }
	rule_text += "0: ";
	rule_text += user_rule;
	rule_text += "\n";
	std::istringstream iss (rule_text);
	auto rules = parse::read_rules(iss);
	auto r = rules["0"];

	uint64_t c = 0;
	for (uint64_t i = 0; i < generator::MAX; ++i) {
		if (i % (1UL << 26) == 0) printf("%.2f%%\n", (double) i / (double) generator::MAX);
		if (r.check(i)) {
			generator::print_chord(i);
			c++;
		}
	}

	std::cout << c << " matches." << std::endl;
}
