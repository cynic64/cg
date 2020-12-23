#ifndef CG_PARSE_H
#define CG_PARSE_H

#include <iostream>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <algorithm>
#include <memory>
#include <iterator>
#include <deque>

#include "chord.hpp"
#include "helpers.hpp"
#include "generator.hpp"

namespace parse {
	enum class UnaryOp {Not};
	enum class BinaryOp {And, Or};

	typedef std::unordered_map<std::string, generator::Rule> Ruleset;

	std::unordered_map<std::string, UnaryOp> UNARY_OPS = {{"!", UnaryOp::Not}};

	std::unordered_map<std::string, BinaryOp> BINARY_OPS = {{"&&", BinaryOp::And},
									    {"||", BinaryOp::Or}};

	std::unordered_map<std::string, int> PRECEDENCE = {{"||", 20},
							   {"&&", 50},
							   {"!", 100}};

	std::vector<std::string> ALL_TOKENS = {"(", ")", "!", "&&", "||"};

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

	// If it's not an operator or a mask, it's a reference
	bool is_reference(std::string& token) {
		if (std::find(ALL_TOKENS.begin(), ALL_TOKENS.end(), token) != ALL_TOKENS.end()) return false;
		if (token.size() != 12) return true;
		return !std::any_of(token.begin(), token.end(), [](auto c){return c >= '0' && c <= '9';});
	}

	generator::Rule new_rule(uint32_t mask) {
		generator::Rule r;
		r.conditions = {mask};
		r.table = {0, 1};

		return r;
	}
	
	generator::Rule new_rule(UnaryOp op, generator::Rule& a) {
		generator::Rule r;
		r.conditions = a.conditions;

		if (op == UnaryOp::Not) {
			for (auto x : a.table) r.table.push_back(!x);
		} else throw;
		
		return r;
	}

	generator::Rule new_rule(BinaryOp op, generator::Rule& a, generator::Rule& b) {
		generator::Rule r;

		// Special case 1: OR-ing two positive base expressions is the
		// same as OR-ing the OR of their masks (what a
		// sentence...). Special case 2: AND-ing two negative base
		// expressions is the same as NAND-ing the OR of their masks. In
		// both cases, we just OR the masks and copy the table.
		bool or_mergeable = op == BinaryOp::Or && a.table == std::vector<bool>{false, true} && b.table == std::vector<bool>{false, true};
		bool and_mergeable = op == BinaryOp::And && a.table == std::vector<bool>{true, false} && b.table == std::vector<bool>{true, false};
		if (or_mergeable || and_mergeable) {
			r.conditions = {a.conditions[0] | b.conditions[0]};
			r.table = a.table;
			return r;
		}

		r.conditions = a.conditions;
		r.conditions.insert(r.conditions.begin(), b.conditions.begin(), b.conditions.end());

		uint32_t M = a.table.size(), N = b.table.size(), m = helpers::log2(M);
		r.table.resize(M*N);

		for (uint64_t i = 0; i < M*N; ++i) {
			uint64_t upper = i >> m;
			uint64_t lower = i & ((1UL << m) - 1);

			if (op == BinaryOp::And) r.table[i] = a.table[lower] && b.table[upper];
			else if (op == BinaryOp::Or) r.table[i] = a.table[lower] || b.table[upper];
			else throw;
		}

		return r;
	}

	// Parses and adds the rule in line to the given Ruleset. Returns false
	// if the line contains references to rules that don't exist yet, true
	// for success. Also returns true if the line is empty or begins with a
	// #. If debug is set, will print the converted token stream.
	bool read_rule(std::string line, Ruleset& rules, bool debug = false) {
		if (line.size() == 0 || line[0] == '#') return true;
		auto colon_pos = line.find(':');
		if (colon_pos == std::string::npos) throw;
		auto name = line.substr(0, colon_pos);
		line = line.substr(colon_pos+1);
		
		std::vector<std::string> tokens;
		std::istringstream iss(line);
		std::copy(std::istream_iterator<std::string>(iss),
			  std::istream_iterator<std::string>(),
			  std::back_inserter(tokens));

		tokens = convert_to_postfix(tokens);

		if (debug) {
			std::cerr << "Rule interpreted as:";
			for (auto const& t : tokens) std::cerr << " " << t;
			std::cerr << std::endl;
		}

		std::vector<generator::Rule> output_stack;
		for (auto& token : tokens) {
			if (UNARY_OPS.find(token) != UNARY_OPS.end()) {
				auto op = UNARY_OPS[token];
				auto a = output_stack.back(); output_stack.pop_back();
				auto r = new_rule(op, a);
				output_stack.push_back(r);
			} else if (BINARY_OPS.find(token) != BINARY_OPS.end()) {
				auto op = BINARY_OPS[token];
				auto a = output_stack.back(); output_stack.pop_back();
				auto b = output_stack.back(); output_stack.pop_back();
				auto r = new_rule(op, a, b);
				output_stack.push_back(r);
			} else if (is_reference(token)) {
				if (rules.find(token) == rules.end()) return false;
				output_stack.push_back(rules[token]);
			} else {
				auto mask = chord::from_string(token);
				output_stack.push_back(new_rule(mask));
			}
		}

		if (output_stack.size() != 1) {
			printf("%lu items left on output stack instead of 1!\n", output_stack.size());
			throw;
		}

		rules[name] = output_stack[0];

		return true;
	}

	// Modifies rules to include the additional rules from the given file
	void read_rules_from_file(std::istream& in, Ruleset& rules) {
		std::deque<std::string> lines;
		for (std::string line; getline(in, line);) lines.push_back(line);

		while (!lines.empty()) {
			auto line = lines.front(); lines.pop_front();
			if (!read_rule(line, rules)) lines.push_back(line);
		}
	}
}

#endif // CG_PARSE_H
