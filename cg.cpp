#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <future>

#include "parse.hpp"
#include "chord.hpp"
#include "generator.hpp"
#include "inspect.hpp"

const auto USAGE = R"(Usage: cg <subcommand> [options]

Subcommands:
 gen [rule]          Generate chords following [rule]
 inspect [chord]     Inspect chords (can read from stdin))";

int main(int argc, char *argv[]) {
	if (argc < 2) {
		std::cerr << USAGE << std::endl;
		exit(1);
	}

	std::string subcommand(argv[1]);
	if (subcommand == "gen") {
		if (argc < 3) {
			std::cerr << "gen requires at least 1 rule!" << std::endl;
			exit(1);
		}
		
		std::string user_rule = "0: ";
		user_rule += argv[2];

		std::ifstream infile("rules.txt");
		if (!infile.is_open()) {
			std::cerr << "Could not open rule file!" << std::endl;
			exit(1);
		}

		parse::Ruleset rules;
		parse::read_rules_from_file(infile, rules);

		if (!parse::read_rule(user_rule, rules, true)) {
			std::cout << "User rule references unspecified rules!" << std::endl;
			exit(1);
		}

		auto rule = rules["0"];
		rule.print();

		rule.print_matching();
	} else if (subcommand == "inspect") {
		if (argc < 3) {
			std::cerr << "Reading from stdin." << std::endl;
			for (std::string line; std::getline(std::cin, line);) {
				auto chord = chord::from_string(line);
				chord::print_mixed(std::cout, chord, false);
				std::cout << ", ";
				inspect::inspect(chord, true);
				std::cout << std::endl;
			}
		} else {
			std::string in(argv[2]);
			auto chord = chord::from_string(in);
			inspect::inspect(chord);
		}
	} else {
		std::cerr << "Unknown subcommand: |" << subcommand << "|" << std::endl;
		exit(1);
	}
}
