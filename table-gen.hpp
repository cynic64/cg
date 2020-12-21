#include <iostream>
#include <vector>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <sstream>
#include <variant>

enum class UnaryOp {Not};
enum class BinaryOp {And, Or};

std::unordered_map<std::string, UnaryOp> UNARY_OPS = {{"!", UnaryOp::Not}};

std::unordered_map<std::string, BinaryOp> BINARY_OPS = {{"&&", BinaryOp::And},
						  {"||", BinaryOp::Or}};

uint64_t log2(uint64_t n) {
	auto r = 0;
	while (n > 1) { r++; n >>= 1; }
	return r;
}

struct BaseExpr;
struct UnaryExpr;
struct BinaryExpr;

typedef std::variant<BaseExpr, UnaryExpr, BinaryExpr> Expr;

struct BaseExpr {
	std::string name;
};

struct UnaryExpr {
	UnaryOp op;
	Expr* a;
};

struct BinaryExpr {
	BinaryOp op;
	Expr* a;
	Expr* b;
};

std::vector<bool> gen_table(Expr* expr) {
	if (std::holds_alternative<BaseExpr>(*expr)) return {0, 1};
	else if (auto e = std::get_if<UnaryExpr>(expr)) {
		auto table = gen_table(e->a);

		for (auto it = table.begin(); it != table.end(); ++it) {
			if (e->op == UnaryOp::Not) *it = !*it;
			else throw;
		}

		return table;
	} else if (auto e = std::get_if<BinaryExpr>(expr)) {		
		auto a_table = gen_table(e->a);
		auto b_table = gen_table(e->b);
		auto M = a_table.size(), N = b_table.size(), n = log2(N);
		std::vector<bool> out (M*N);

		if (M*N > 64) throw;

		for (uint64_t i = 0; i < M*N; ++i) {
			uint64_t upper = i >> n;
			uint64_t lower = i & ((1UL << n) - 1);

			if (e->op == BinaryOp::And) out[i] = a_table[upper] && b_table[lower];
			else if (e->op == BinaryOp::Or) out[i] = a_table[upper] || b_table[lower];
			else throw;
		}

		return out;
	} else throw;
}

// Expects tokens in postfix order. Heap-allocates everything...
Expr* from_tokens(std::vector<std::string> tokens) {
	std::vector<Expr*> out;
	
	for (auto token : tokens) {
		if (UNARY_OPS.find(token) != UNARY_OPS.end()) {
			auto op = UNARY_OPS[token];
			auto a = out.back(); out.pop_back();
			Expr* e = new Expr {std::in_place_index<1>, UnaryExpr {op, a}};
			out.push_back(e);
		} else if (BINARY_OPS.find(token) != BINARY_OPS.end()) {
			auto op = BINARY_OPS[token];
			auto b = out.back(); out.pop_back();
			auto a = out.back(); out.pop_back();
			Expr* e = new Expr {std::in_place_index<2>, BinaryExpr {op, a, b}};
			out.push_back(e);
		} else {
			Expr* e = new Expr {std::in_place_index<0>, BaseExpr {token}};
			out.push_back(e);
		}
	}

	if (out.size() != 1) {
		printf("%lu items in out instead of 1!\n", out.size());
		throw;
	}

	return out[0];
}

void print_expr(Expr* expr, bool newline = true) {
	if (auto e = std::get_if<BaseExpr>(expr)) {
		std::cout << e->name;
	} else if (auto e = std::get_if<UnaryExpr>(expr)) {
		std::cout << '(';
		for (auto [token, op] : UNARY_OPS) if (op == e->op) std::cout << token;

		print_expr(e->a, false);
		std::cout << ')';
	} else if (auto e = std::get_if<BinaryExpr>(expr)) {
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

void print_table(std::vector<bool> table) {
	auto vars = log2(table.size());

	for (uint64_t i = 0; i < table.size(); ++i) {
		for (uint64_t v = 0; v < vars; ++v)
			std::cout << ((char) (v+'A')) << ": " << (i >> (vars-v-1) & 1) << " ";
		std::cout << "= ";
		std::cout << table[i] << std::endl;
	}
}
