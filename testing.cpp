#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <future>

#include "constants.hpp"
#include "parse.hpp"

const auto THREADS = 8;
const uint64_t COUNT_PER_THREAD = MAX / THREADS;

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

	std::vector<std::future<std::pair<uint64_t, uint64_t>>> futures(THREADS);
	std::vector<std::vector<uint64_t>> buffers(THREADS, std::vector<uint64_t>(generator::BUFSIZE));

	for (auto t = 0; t < THREADS; ++t) {
		uint64_t start = COUNT_PER_THREAD * t;
		uint64_t stop = start + COUNT_PER_THREAD;
		futures[t] = std::async(std::launch::async, &generator::Rule::check_range, &rule,
					start, stop, std::ref(buffers[t]));
	}

	auto still_running = THREADS;
	while (still_running) {
		for (auto t = 0; t < THREADS; ++t) {
			if (!futures[t].valid()) continue;

			auto [count, stop_pos] = futures[t].get();
			uint64_t all_done = (t+1) * COUNT_PER_THREAD;

			if (stop_pos == all_done) {
				still_running--;
				for (uint64_t i = 0; i < count; ++i) helpers::print_chord(buffers[t][i]);
				continue;
			}

			for (uint64_t i = 0; i < count; ++i) helpers::print_chord(buffers[t][i]);
			futures[t] = std::async(std::launch::async, &generator::Rule::check_range, &rule,
						stop_pos, all_done, std::ref(buffers[t]));
		}
	}
}
