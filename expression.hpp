#ifndef CG_EXPRESSION_H
#define CG_EXPRESSION_H

#include <iostream>
#include <vector>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <sstream>
#include <variant>
#include <memory>

namespace expression {
	enum class UnaryOp {Not};
	enum class BinaryOp {And, Or};

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
		std::shared_ptr<Expr> a;
	};

	struct BinaryExpr {
		BinaryOp op;
		std::shared_ptr<Expr> a;
		std::shared_ptr<Expr> b;
	};

	std::vector<bool> gen_table(Expr* expr) {
		if (std::holds_alternative<BaseExpr>(*expr)) return {0, 1};
		else if (auto e = std::get_if<UnaryExpr>(expr)) {
			auto table = gen_table(e->a.get());

			for (auto it = table.begin(); it != table.end(); ++it) {
				if (e->op == UnaryOp::Not) *it = !*it;
				else throw;
			}

			return table;
		} else if (auto e = std::get_if<BinaryExpr>(expr)) {		
			auto a_table = gen_table(e->a.get());
			auto b_table = gen_table(e->b.get());
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

	void print_table(std::vector<bool> table) {
		auto vars = log2(table.size());

		for (uint64_t i = 0; i < table.size(); ++i) {
			for (uint64_t v = 0; v < vars; ++v)
				std::cout << ((char) (v+'A')) << ": " << (i >> (vars-v-1) & 1) << " ";
			std::cout << "= ";
			std::cout << table[i] << std::endl;
		}
	}
}

#endif // CG_EXPRESSION_H
