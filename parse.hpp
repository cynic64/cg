#ifndef CG_PARSE_H
#define CG_PARSE_H

#include <iostream>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <algorithm>

#include "expression.hpp"

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

			print_expr(e->a, false);
			std::cout << ')';
		} else if (auto e = std::get_if<expression::BinaryExpr>(expr)) {
			std::cout << '(';
			print_expr(e->a, false);
			std::cout << ' ';
			for (auto [token, op] : BINARY_OPS) if (op == e->op) std::cout << token;
			std::cout << ' ';
			print_expr(e->b, false);
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
	expression::Expr* tokens_to_expr(std::vector<std::string> tokens) {
		std::vector<expression::Expr*> out;
	
		for (auto token : tokens) {
			if (UNARY_OPS.find(token) != UNARY_OPS.end()) {
				auto op = UNARY_OPS[token];
				auto a = out.back(); out.pop_back();
				auto e = new expression::Expr {std::in_place_index<1>, expression::UnaryExpr {op, a}};
				out.push_back(e);
			} else if (BINARY_OPS.find(token) != BINARY_OPS.end()) {
				auto op = BINARY_OPS[token];
				auto b = out.back(); out.pop_back();
				auto a = out.back(); out.pop_back();
				auto e = new expression::Expr {std::in_place_index<2>, expression::BinaryExpr {op, a, b}};
				out.push_back(e);
			} else {
				auto e = new expression::Expr {std::in_place_index<0>, expression::BaseExpr {token}};
				out.push_back(e);
			}
		}

		if (out.size() != 1) {
			printf("%lu items in out instead of 1!\n", out.size());
			throw;
		}

		return out[0];
	}
}

#endif // CG_PARSE_H
