#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <future>

#include "parse.hpp"
#include "chord.hpp"

const auto USAGE = R"(Usage: cg <subcommand> [options]

Subcommands:
 gen [rule]          Generate chords following [rule]
 inspect             Inspect chords from stdin)";

int main(int argc, char *argv[]) {
	if (argc < 2) {
		std::cerr << USAGE << std::endl;
		exit(1);
	}

	std::string user_rule = "0: ";
	user_rule += argv[1];

	std::ifstream infile("rules.txt");
	if (!infile.is_open()) throw;

	parse::Ruleset rules;
	parse::read_rules_from_file(infile, rules);

	if (!parse::read_rule(user_rule, rules, true)) {
		std::cout << "User rule references unspecified rules!" << std::endl;
		throw;
	}

	auto rule = rules["0"];
	rule.print();

	rule.print_matching();
}
