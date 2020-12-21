#ifndef CG_PARSE_H
#define CG_PARSE_H

#include <iostream>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <algorithm>
#include <memory>

#include "expression.hpp"
#include "generator.hpp"

namespace parse {
	std::unordered_map<std::string, expression::UnaryOp> UNARY_OPS = {{"!", expression::UnaryOp::Not}};

	std::unordered_map<std::string, expression::BinaryOp> BINARY_OPS = {{"&&", expression::BinaryOp::And},
								{"||", expression::BinaryOp::Or}};

	std::unordered_map<std::string, int> PRECEDENCE = {{"||", 20},
							   {"&&", 50},
							   {"!", 100}};

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

	std::vector<std::string> convert_to_postfix(std::string& in) {
		std::vector<std::string> ops;
		std::vector<std::string> out;

		std::istringstream stream(in);
		for (std::string token; stream >> token;) {
			if (PRECEDENCE.find(token) != PRECEDENCE.end()) {
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
}

#endif // CG_PARSE_H
