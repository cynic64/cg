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
	auto expr = parse::tokens_to_expr(tokens);

	parse::print_expr(expr);
	auto table = expression::gen_table(expr);
	expression::print_table(table);
}
