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
 inspect [what]      Inspect chords (what = fingerings, intervals...)
 transpose [offset]  Transpose chords up and down

<inspect> and <transpose> read from stdin.
)";

int main(int argc, char *argv[]) {
	if (argc < 2) {
		std::cerr << USAGE;
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
			std::cerr << "User rule references unspecified rules!" << std::endl;
			exit(1);
		}

		auto rule = rules["0"];
		rule.print();

		rule.print_matching();
	} else if (subcommand == "transpose") {
		std::vector<int> shifts;
		if (argc < 3) {
			std::cerr << "No shifts given, will try everything possible." << std::endl;
			for (auto i = chord::LOWEST_NOTE; i <= chord::HIGHEST_NOTE; ++i) shifts.push_back(i);
		} else {
			for (auto i = 2; i < argc; ++i) shifts.push_back(std::stoi(argv[i]));
			std::cerr << "Shifts: " << helpers::fmt_vector(shifts) << std::endl;
		}

		for (std::string line; std::getline(std::cin, line);) {
			chord::Chord orig(line);
			for (auto s : shifts) {
				auto c = orig;
				c.transpose(s);
				if (!c.within_range()) continue;
				std::cout << c.fmt() << std::endl;
			}
		}
	} else if (subcommand == "inspect") {
		std::vector<inspect::Key> keys;
		if (argc < 3) {
			keys = inspect::ALL_KEYS;
			std::cerr << "No inspection keys given." << std::endl;
		} else {
			for (auto i = 2; i < argc; ++i) keys.push_back(inspect::from_string(argv[i]));
		}

		std::cerr << "Inspecting: ";
		for (auto k : keys) std::cerr << inspect::to_string(k) << " ";
		std::cerr << std::endl;

		for (std::string line; getline(std::cin, line);) {
			chord::Chord chord(line);
			auto details = inspect::inspect(chord, keys);

			for (auto k : keys) {
				std::cout << inspect::to_string(k) << ": ";
				std::cout << details[k] << ", ";
			}
			std::cout << "chord: " << chord.fmt();
			std::cout << std::endl;
		}
	} else {
		std::cerr << "Unknown subcommand: |" << subcommand << "|" << std::endl;
		exit(1);
	}
}
