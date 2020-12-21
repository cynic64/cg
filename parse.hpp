#ifndef CG_PARSE_H
#define CG_PARSE_H

#include <iostream>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <algorithm>
#include <memory>
#include <iterator>
#include <set>

#include "expression.hpp"
#include "generator.hpp"

namespace parse {
	std::unordered_map<std::string, expression::UnaryOp> UNARY_OPS = {{"!", expression::UnaryOp::Not}};

	std::unordered_map<std::string, expression::BinaryOp> BINARY_OPS = {{"&&", expression::BinaryOp::And},
								{"||", expression::BinaryOp::Or}};

	std::unordered_map<std::string, int> PRECEDENCE = {{"||", 20},
							   {"&&", 50},
							   {"!", 100}};

	std::vector<std::string> ALL_TOKENS = {"(", ")", "!", "&&", "||"};

	void print_expr(expression::Expr* expr, bool newline = true) {
		if (auto e = std::get_if<expression::BaseExpr>(expr)) {
			std::cout << e->name;
		} else if (auto e = std::get_if<expression::UnaryExpr>(expr)) {
			std::cout << '(';
			for (auto [token, op] : UNARY_OPS) if (op == e->op) std::cout << token;

			print_expr(e->a.get(), false);
			std::cout << ')';
		} else if (auto e = std::get_if<expression::BinaryExpr>(expr)) {
			std::cout << '(';
			print_expr(e->a.get(), false);
			std::cout << ' ';
			for (auto [token, op] : BINARY_OPS) if (op == e->op) std::cout << token;
			std::cout << ' ';
			print_expr(e->b.get(), false);
			std::cout << ')';
		}
	
		if (newline) std::cout << std::endl;
	}

	std::vector<std::string> convert_to_postfix(std::vector<std::string>& tokens) {
		std::vector<std::string> ops;
		std::vector<std::string> out;

		for (auto& token : tokens) {
			if (PRECEDENCE.find(token) != PRECEDENCE.end()) {
				while (ops.size() && ops.back() != "(" && PRECEDENCE[ops.back()] > PRECEDENCE[token]) {
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

	// Expects tokens in postfix order. Heap-allocates all the exprs! Sorry :p
	generator::Rule tokens_to_rule(std::vector<std::string> tokens) {
		std::vector<std::shared_ptr<expression::Expr>> expressions;
		std::vector<generator::Condition> conditions;
	
		for (auto token : tokens) {
			if (UNARY_OPS.find(token) != UNARY_OPS.end()) {
				auto op = UNARY_OPS[token];
				auto a = expressions.back(); expressions.pop_back();
				std::shared_ptr<expression::Expr> e (new expression::Expr {std::in_place_index<1>, expression::UnaryExpr {op, a}});
				expressions.push_back(e);
			} else if (BINARY_OPS.find(token) != BINARY_OPS.end()) {
				auto op = BINARY_OPS[token];
				auto b = expressions.back(); expressions.pop_back();
				auto a = expressions.back(); expressions.pop_back();
				std::shared_ptr<expression::Expr> e (new expression::Expr {std::in_place_index<2>, expression::BinaryExpr {op, a, b}});
				expressions.push_back(e);
			} else {
				std::shared_ptr<expression::Expr> e (new expression::Expr {std::in_place_index<0>, expression::BaseExpr {token}});
				expressions.push_back(e);
				conditions.push_back({token});
			}
		}

		if (expressions.size() != 1) {
			printf("%lu expressions left instead of 1!\n", expressions.size());
			throw;
		}

		std::cout << "Parsed expression:" << std::endl;
		print_expr(expressions[0].get());

		auto table = expression::gen_table(expressions[0].get());

		return {conditions, table};
	}

	template<class T>
	std::unordered_map<std::string, generator::Rule> read_rules(T& in) {
		std::unordered_map<std::string, std::vector<std::string>> rules_tokenized;
		std::set<std::string> unexpanded_rules;

		for (std::string line; std::getline(in, line);) {
			if (line[0] == '#') continue;
			std::cout << "Reading: |" << line << "|" << std::endl;

			auto colon_pos = line.find(':');
			if (colon_pos == std::string::npos) {
				printf("No colon on line!\n");
				throw;
			}

			auto name = line.substr(0, colon_pos);
			std::cout << "\tName: " << name << std::endl;

			std::vector<std::string> tokens;
			std::istringstream definition (line.substr(colon_pos+1));
			std::copy(std::istream_iterator<std::string>(definition),
				  std::istream_iterator<std::string>(),
				  std::back_inserter(tokens));

			rules_tokenized[name] = tokens;
			unexpanded_rules.insert(name);
		}

		while (!unexpanded_rules.empty()) {
			auto progress = 0;
			for (auto& [name, _] : rules_tokenized) {
				if (unexpanded_rules.find(name) == unexpanded_rules.end()) continue;
				
				auto is_expanded = true;

				auto& tokens = rules_tokenized[name];
				size_t token_idx = 0;
				while (token_idx < tokens.size()) {
					auto token = tokens[token_idx];
					if (std::find(ALL_TOKENS.begin(), ALL_TOKENS.end(), token) == ALL_TOKENS.end()) {
						// If it's not a token, it's
						// either a mask or something we
						// have to expand
						if (token.size() == generator::INTERVALS
						    && std::all_of(token.begin(), token.end(), [](auto c){ return c >= '0' && c <= '0' + generator::BASE; })) {
							token_idx++;
							continue;
						}

						is_expanded = false;

						if (unexpanded_rules.find(token) == unexpanded_rules.end()) {
							auto expansion = rules_tokenized[token];
							std::vector<std::string> wrapped_expansion {"("};
							wrapped_expansion.insert(wrapped_expansion.end(), expansion.begin(), expansion.end());
							wrapped_expansion.push_back(")");

							tokens.erase(tokens.begin() + token_idx);
							tokens.insert(tokens.begin() + token_idx, wrapped_expansion.begin(), wrapped_expansion.end());

							progress++;
						}
					}

					token_idx++;
				}

				if (!is_expanded) std::cout << "Have to expand: " << name << std::endl;

				if (is_expanded) {
					unexpanded_rules.erase(name);
					progress++;
				}
			}

			if (progress == 0) {
				std::cout << "Cannot expand remaining rules:" << std::endl;
				for (auto& name : unexpanded_rules) std::cout << "\t" << name << std::endl;
				throw;
			}

			std::cout << "Progress: " << progress << std::endl;
		}

		std::cout << std::endl << "Expanded rules:" << std::endl;
		for (auto& [name, tokens] : rules_tokenized) {
			std::cout << "\t" << name << ": ";
			for (auto& t : tokens) std::cout << t << " ";
			std::cout << std::endl;
		}

		for (auto& [name, tokens] : rules_tokenized) tokens = convert_to_postfix(tokens);

		std::cout << std::endl << "Processed rules:" << std::endl;
		for (auto& [name, tokens] : rules_tokenized) {
			std::cout << "\t" << name << ": ";
			for (auto& t : tokens) std::cout << t << " ";
			std::cout << std::endl;
		}

		std::unordered_map<std::string, generator::Rule> rules;
		for (auto& [name, tokens] : rules_tokenized) rules[name] = tokens_to_rule(tokens);

		for (auto& [name, rule] : rules) {
			std::cout << name << std::endl;
			rule.print();
			std::cout << std::endl;
		}
			
		return rules;
	}
}

#endif // CG_PARSE_H
