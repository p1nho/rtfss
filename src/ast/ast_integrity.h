#ifndef RTFSS_DEF_AST_INTEGRITY
#define RTFSS_DEF_AST_INTEGRITY

#include "ast.h"
#include <cassert>

//Checks if ast is really a tree
class ast_integrity{

	public:
		ast_integrity(std::shared_ptr<ast> tree):
			tree(tree){}
		bool check();

	private:
		std::shared_ptr<ast> tree;
		std::vector<std::shared_ptr<ast>> visited;
		std::vector<std::shared_ptr<ast>> visit_stack;

};

#endif
