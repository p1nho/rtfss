#include "ast_integrity.h"

bool ast_integrity::check(){
	visited.clear();
	visit_stack.clear();

	visit_stack.push_back(tree);

	std::shared_ptr<ast> curr;
	while(visit_stack.size()!=0){
		curr=visit_stack.back();
		visit_stack.pop_back();

		if(std::find(visited.begin(),visited.end(),curr)!=visited.end()){
			//Already visited this node
			std::cerr<<"ast_integrity ERROR: already visited"<<std::endl;

			assert(false);
			return false;
		}
		visited.push_back(curr);

		std::vector<std::shared_ptr<ast>> to_add(curr->get_children());
		visit_stack.insert(visit_stack.end(),to_add.begin(),to_add.end());
	}

	return true;
}
