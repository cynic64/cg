#include <iostream>

#include "table-gen.hpp"
#include "parse.hpp"

int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("Not enough args\n");
		exit(1);
	}

	std::string input(argv[1]);
	auto tokens = convert_to_postfix(input);
	auto expr = from_tokens(tokens);

	print_expr(expr);
	auto table = gen_table(expr);
	print_table(table);
}
