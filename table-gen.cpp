#include <iostream>
#include <vector>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <sstream>

// Parsing is easier if we can store parentheses as operators... sorry!
enum class Op {And, Or, OpenParen};

std::unordered_map<std::string, Op> OPERATORS = {{"&&", Op::And},
						       {"||", Op::Or},
						       {"(", Op::OpenParen}};
std::unordered_map<Op, int> PRECEDENCE = {{Op::And, 50},
							 {Op::Or, 20},
							 {Op::OpenParen, 999}};

uint64_t log2(uint64_t n) {
	auto r = 0;
	while (n > 1) { r++; n >>= 1; }
	return r;
}

struct Expr {
	Op op;
	Expr* a;
	Expr* b;

	Expr() {}
	Expr(Op op) : op(op), a(nullptr), b(nullptr) {}
	Expr(Op op, Expr* a) : op(op), a(a), b(nullptr) {}
	Expr(Op op, Expr* a, Expr* b) : op(op), a(a), b(b) {}

	Expr(std::string& s) {
		std::vector<Op> ops;
		std::vector<Expr*> out;

		std::istringstream stream(s);
		for (std::string token; stream >> token;) {
			if (OPERATORS.find(token) != OPERATORS.end()) {
				while (ops.size() && PRECEDENCE[ops.back()] > PRECEDENCE[OPERATORS[token]]) {
					auto b = out.back(); out.pop_back();
					auto a = out.back(); out.pop_back();
					out.push_back(new Expr(ops.back(), a, b)); ops.pop_back();
				}
				ops.push_back(OPERATORS[token]);
			} else if (token == ")") {
				while (ops.back() != Op::OpenParen) {
					auto b = out.back(); out.pop_back();
					auto a = out.back(); out.pop_back();
					out.push_back(new Expr(ops.back(), a, b)); ops.pop_back();
				}
				ops.pop_back();
			} else out.push_back(nullptr);
		}

		while (ops.size()) {
			auto b = out.back(); out.pop_back();
			auto a = out.back(); out.pop_back();
			out.push_back(new Expr(ops.back(), a, b)); ops.pop_back();
		}

		if (out.size() != 1) throw;

		a = out[0]->a;
		b = out[0]->b;
		op = out[0]->op;
		delete out[0];
	}

	// Returns how many variables (leaves) were printed
	int print(bool newline = true, char next_v = 'A') {
		auto initial_v = next_v;
		
		std::cout << '(';

		// Print a
		if (a) next_v += a->print(false, next_v);
		else { std::cout << next_v; next_v++; }

		// Print op
		if (op == Op::And) std::cout << " && ";
		else if (op == Op::Or) std::cout << " || ";
		else throw;

		// Print b
		if (b) next_v += b->print(false, next_v);
		else { std::cout << next_v; next_v++; };

		// Finish
		std::cout << ')';
		if (newline) std::cout << std::endl;

		return next_v - initial_v;
	}

	std::vector<bool> gen_table() {
		std::vector<bool> a_table = a ? a->gen_table() : std::vector<bool>{0, 1},
			b_table = b ? b->gen_table() : std::vector<bool>{0, 1};
		auto M = a_table.size(), N = b_table.size(), n = log2(N);
		std::vector<bool> out(M*N);

		for (uint64_t i = 0; i < M*N; ++i) {
			uint64_t upper = i >> n;
			uint64_t lower = i & ((1UL << n) - 1);

			if (op == Op::And) out[i] = a_table[upper] && b_table[lower];
			else if (op == Op::Or) out[i] = a_table[upper] || b_table[lower];
			else throw;
		}

		return out;
	}
};

void print_table(std::vector<bool> table) {
	auto vars = log2(table.size());

	for (uint64_t i = 0; i < table.size(); ++i) {
		for (uint64_t v = 0; v < vars; ++v)
			std::cout << ((char) (v+'A')) << ": " << (i >> (vars-v-1) & 1) << " ";
		std::cout << "= ";
		std::cout << table[i] << std::endl;
	}
}

int main() {
	Expr a(Op::Or);
	Expr b(Op::And, &a);
	Expr c(Op::Or, &b);
	c.print();
	print_table(c.gen_table());

	std::string s = "A && B || C && D";
	Expr d(s);
	d.print();
	print_table(d.gen_table());
}
