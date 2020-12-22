#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <future>

#include "constants.hpp"
#include "parse.hpp"

const auto THREADS = 16;

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

	if (!parse::read_rule(user_rule, rules, true)) {
		std::cout << "User rule references unspecified rules!" << std::endl;
		throw;
	}

	auto rule = rules["0"];
	rule.print();

	std::array<std::future<uint64_t>, THREADS> futures;
	std::array<std::array<uint64_t, generator::BUFSIZE>, THREADS> buffers;

	uint64_t start = 0;
	while (start < MAX) {
		for (auto t = 0; t < THREADS; ++t) {
			futures[t] = std::async(std::launch::async, &generator::Rule::check_range,
						&rule, start, generator::BUFSIZE, 1, std::ref(buffers[t]));
			start += generator::BUFSIZE;
		}

		for (auto t = 0; t < THREADS; ++t) {
			auto count = futures[t].get();
			for (uint64_t i = 0; i < count; ++i) helpers::print_chord(buffers[t][i]);
		}
	}
}
