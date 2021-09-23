#ifndef RTFSS_DEF_AST_PROPERTY
#define RTFSS_DEF_AST_PROPERTY

#include "ast.h"

//TODO connect this to ast so the duplication works correctly?

template <class T> class ast_property{

	private:
		std::map<std::shared_ptr<ast>,T,std::owner_less<std::shared_ptr<ast>>> nodes;
		std::shared_ptr<ast> super_parent;
		bool check;

	public:
		ast_property(bool check_insert=true):
			check(check_insert){};

		void put(std::shared_ptr<ast> node,T &data);
		T* get(std::shared_ptr<ast> node);
		bool erase(std::shared_ptr<ast> node);
		bool check_same_tree(std::shared_ptr<ast> node);
		static std::shared_ptr<ast> get_ast_super(std::shared_ptr<ast> node);
};

#include "ast_property.tcc" //Implementation

#endif
