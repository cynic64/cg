#include <iostream>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <algorithm>

std::vector<std::string> OPERATORS = {"&&", "||", "'"};

std::unordered_map<std::string, int> PRECEDENCE = {{"||", 20},
						   {"&&", 50},
						   {"'", 100},
						   {"(", 999}};

std::vector<std::string> convert_to_postfix(std::string& in) {
	std::vector<std::string> ops;
	std::vector<std::string> out;

	std::istringstream stream(in);
	for (std::string token; stream >> token;) {
		auto is_binary_op = std::find(OPERATORS.begin(), OPERATORS.end(), token) != OPERATORS.end();

		if (is_binary_op) {
			while (ops.size() && PRECEDENCE[ops.back()] > PRECEDENCE[token] && ops.back() != "(") {
				out.push_back(ops.back());
				ops.pop_back();
			}
			ops.push_back(token);
		} else if (token == "(") {
			ops.push_back(token);
		} else if (token == ")") {
			while (ops.back() != "(") {
				out.push_back(ops.back());
				ops.pop_back();
			}
			ops.pop_back();
		} else out.push_back(token);
	}

	while (ops.size()) {
		out.push_back(ops.back());
		ops.pop_back();
	}

	return out;
}

int main(int argc, char *argv[]) {
	if (argc < 2) exit(1);

	std::string input(argv[1]);
	auto converted_input = convert_to_postfix(input);

	for (auto token : converted_input) std::cout << token << ' ';
	std::cout << std::endl;
}

