#include <iostream>

#include "expression.hpp"
#include "parse.hpp"

int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("Not enough args\n");
		exit(1);
	}

	std::string input(argv[1]);
	auto tokens = parse::convert_to_postfix(input);
	auto rule = parse::tokens_to_rule(tokens);

	rule.print();

	for (uint64_t i = 0; i < generator::MAX; ++i) {
		if (rule.check(i)) generator::print_chord(i);
	}
}
