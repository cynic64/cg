#include "table-gen.hpp"

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

	s = "A && !B";
	d = {s};
	d.print();
	print_table(d.gen_table());
}
