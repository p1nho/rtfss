#include "assign_trimmer.h"
#include <memory>

//TODO actually trim the tree

std::string assign_trimmer::to_string(){
	std::stringstream ss;

	int i=0;
	for(auto it=deps.begin();it!=deps.end();it++,i++){
		ss<<i<<":{";

		for(auto it1=it->begin();it1!=it->end();it1++){
			ss<<*it1<<",";
		}

		ss<<"}"<<std::endl;
	}

	return ss.str();
}

void assign_trimmer::check_sanity(){
	std::vector<bool> visited(deps.size(),false);

	std::vector<std::vector<unsigned int>> stacks;		
	for(unsigned int i=0;i<visited.size();i++){
		if(visited[i]==true){ //already visited
			continue;
		}

		//Depth first traversal
		stacks.push_back({i});
		while(stacks.size()!=0){
			std::vector<unsigned int> curr_stack=stacks.back();
			stacks.pop_back();
			unsigned int curr=curr_stack.back();
			visited[curr]=true;

			if(std::find(curr_stack.begin(),curr_stack.end()-1,curr)!=curr_stack.end()-1){ //Already visited
				error_handler::stream_dep_loop(vcscope,curr_stack);
				//std::cerr<<"LOOP FOUND"<<std::endl;
				return;
			}

			//For each connection, duplicate stack and add new node
			std::set<unsigned int> &next(deps[curr]);
			for(auto it=next.begin();it!=next.end();it++){
				std::vector<unsigned int> next_stack(curr_stack);
				next_stack.push_back(*it);
				stacks.push_back(next_stack);
			}
		}
	}
	//std::cerr<<"No loops found"<<std::endl;
}

void assign_trimmer::visit_start(std::shared_ptr<ast_start> node){ 
	//Not relevant
	visit(node->get_children());

	//std::cerr<<to_string()<<std::endl;
	
	//Check if there are loops
	check_sanity();
}

void assign_trimmer::visit_include(std::shared_ptr<ast_include> node){ 
	//Not relevant
	(void) node;
	//visit(node->get_children());
}

void assign_trimmer::visit_cblk_decl(std::shared_ptr<ast_cblk_decl> node){ 
	//Visit only code, in _REVERSE_ order
	std::vector<std::shared_ptr<ast>> rev_code(node->get_code());
	std::reverse(rev_code.begin(),rev_code.end());

	visit(rev_code);
}

void assign_trimmer::visit_cblk_inst(std::shared_ptr<ast_cblk_inst> node){ 
	//FIXME FIXME
	(void) node;
	//visit(node->get_children());
}

void assign_trimmer::visit_s_cblk_inst(std::shared_ptr<ast_s_cblk_inst> node){ 
	//FIXME FIXME
	(void) node;
	//visit(node->get_children());
}

void assign_trimmer::visit_pulse(std::shared_ptr<ast_pulse> node){ 
	//Not relevant
	(void) node;
	//visit(node->get_children());
}

void assign_trimmer::visit_if(std::shared_ptr<ast_if> node){ 
	//FIXME FIXME
	(void) node;
	//IFs require duplication
}

void assign_trimmer::visit_for(std::shared_ptr<ast_for> node){ 
	//FIXME FIXME
	(void) node;
	//FORs require duplication
}

void assign_trimmer::visit_decl(std::shared_ptr<ast_decl> node){ 

	//Skip if there is no assignment involved
	if(node->get_attrib()==nullptr){
		return;
	}

	unsigned int this_var(*var_id.get(node->get_var()));

	//Error checking
	//Check if this assignment is to an IN stream
	if(vcscope->get()[this_var]->tk==var_property::IN){
		error_handler::assign_in_stream(vcscope->get()[this_var]->name);
	}

	//Check if this assignment is for the default value of a stream
	std::shared_ptr<ast> ins(node->get_var()->get_instant());
	if(ins!=nullptr){ //Has time index, check it

		//Error checking
		if(ins->get_op()!=ast::NUM){ //Couldnt be solved
			error_handler::inst_not_const(node->get_var()->get_name());
		}

		std::shared_ptr<ast_num> num(std::static_pointer_cast<ast_num>(ins));
	
		//Error checking
		if(!num->get_is_int()){
			error_handler::inst_not_int(node->get_var()->get_name(),num->get_double());
		}

		if(num->get_int()<0){ //the time index is valid, count 

			//Check if this instant has already been defaulted
			auto p(def_val_set[this_var].find(-(num->get_int()+1)));
			if(p!=def_val_set[this_var].end()){
				//It has. Remove assignment and throw warning
				warning_handler::def_val_stream_ignored(node->get_var()->get_name(),num->get_int());
				node->get_parent().lock()->cut_child(node); //Remove from tree
			}
			else{
				//Set as visited and keep going
				def_val_set[this_var][-(num->get_int()+1)]=true;
			}

			return;
		}
	}

	std::set<unsigned int> &this_dep(deps[this_var]);

	//If this_dep has deps, and neither are itself, skip declaration
	auto self_it(std::find(this_dep.begin(),this_dep.end(),this_var));
	if(self_it==this_dep.end()){
		if(this_dep.size()!=0){
			node->get_parent().lock()->cut_child(node); //Remove from tree
			return;
		}
	}
	else{
		//Remove self dependency
		this_dep.erase(self_it);
	}


	std::set<unsigned int> new_dep;
	std::set<unsigned int> *new_dep_ptr(&new_dep);
	dep_prop.put(node,new_dep_ptr);

	//Traverse the rest of the declaration
	visit(node->get_attrib());

	//Add new found dependencies to this_dep
	this_dep.insert(new_dep.begin(),new_dep.end());

}

void assign_trimmer::visit_assign(std::shared_ptr<ast_assign> node){ 
	unsigned int this_var(*var_id.get(node->get_var()));

	//Error checking
	//Check if this assignment is to an IN stream
	if(vcscope->get()[this_var]->tk==var_property::IN){
		error_handler::assign_in_stream(vcscope->get()[this_var]->name);
	}

	//Check if this assignment is for the default value of a stream
	std::shared_ptr<ast> ins(node->get_var()->get_instant());
	if(ins!=nullptr){ //Has time index, check it

		//Error checking
		if(ins->get_op()!=ast::NUM){ //Couldnt be solved
			error_handler::inst_not_const(node->get_var()->get_name());
		}

		std::shared_ptr<ast_num> num(std::static_pointer_cast<ast_num>(ins));
	
		//Error checking
		if(!num->get_is_int()){
			error_handler::inst_not_int(node->get_var()->get_name(),num->get_double());
		}

		if(num->get_int()<0){ //the time index is valid, count 

			//Check if this instant has already been defaulted
			auto p(def_val_set[this_var].find(-(num->get_int()+1)));
			if(p!=def_val_set[this_var].end()){
				//It has. Remove assignment and throw warning
				warning_handler::def_val_stream_ignored(node->get_var()->get_name(),num->get_int());
				node->get_parent().lock()->cut_child(node); //Remove from tree
			}
			else{
				//Set as visited and keep going
				def_val_set[this_var][-(num->get_int()+1)]=true;
			}

			return;
		}
	}

	std::set<unsigned int> &this_dep(deps[this_var]);

	//If this_dep has deps, and neither are itself, skip declaration
	auto self_it(std::find(this_dep.begin(),this_dep.end(),this_var));
	if(self_it==this_dep.end()){
		if(this_dep.size()!=0){
			node->get_parent().lock()->cut_child(node); //Remove from tree
			return;
		}
	}
	else{
		//Remove self dependency
		this_dep.erase(self_it);
	}

	//Add self dependency if the attribution isnt a simple "="
	if(node->get_assign_op().get_op()!=assign_op::EQUAL){
		//TODO detach and atach elsewhere
		this_dep.insert(this_var);
	}

	std::set<unsigned int> new_dep;
	std::set<unsigned int> *new_dep_ptr(&new_dep);
	dep_prop.put(node,new_dep_ptr);

	//Traverse the rest of the declaration
	visit(node->get_attrib());

	//Add new found dependencies to this_dep
	this_dep.insert(new_dep.begin(),new_dep.end());
}

void assign_trimmer::visit_cast(std::shared_ptr<ast_cast> node){ 
	//Propagate dep_prop
	std::set<unsigned int>* dp(*(dep_prop.get(node->get_parent().lock())));
	dep_prop.put(node,dp);
	
	//Visit normally
	visit(node->get_children());
}

void assign_trimmer::visit_arrinit(std::shared_ptr<ast_arrinit> node){ 
	//FIXME how to handle this
	visit(node->get_children());
}

void assign_trimmer::visit_num(std::shared_ptr<ast_num> node){ 
	//Not relevant
	(void) node;
	//visit(node->get_children());
}

void assign_trimmer::visit_id(std::shared_ptr<ast_id> node){ 
	//Get dep_prop and insert self to the set
	std::set<unsigned int>* dp(*(dep_prop.get(node->get_parent().lock())));

	//Get this_var
	unsigned int this_var(*var_id.get(node));

	//Insert, if appliable, this_var to dep_prop
	if(node->get_instant()==nullptr){ //Not time indexed, counts as normal stream
		dp->insert(this_var);
	}
	else{ //Has time index, check it
		std::shared_ptr<ast_num> num(std::static_pointer_cast<ast_num>(node->get_instant()));
		bool zero_positive(num->get_is_int()?num->get_int()>=0:num->get_double()>=0);

		if(zero_positive){ //same as not having time index, counts as normal stream
			dp->insert(this_var);
		}
		//else, the time index is valid, thus this stream is always read only
		//In other words, it cannot generate cyclic depedencies

		//But, if pid is equal to zero, aka its a const stream, raise error
		//Error checking
		if(vcscope->get()[this_var]->pid==0){
			error_handler::const_stream_ins(vcscope->get()[this_var]->name);
		}
	}

	//End of the line
	//visit(node->get_children());
}

//Overall bin/unary
void assign_trimmer::visit_bin(std::shared_ptr<ast_bin_arith> node){
	//Propagate dep_prop
	std::set<unsigned int>* dp(*(dep_prop.get(node->get_parent().lock())));
	dep_prop.put(node,dp);
	
	//Visit normally
	visit(node->get_children());
}

void assign_trimmer::visit_unary(std::shared_ptr<ast_unary_arith> node){
	//Propagate dep_prop
	std::set<unsigned int>* dp(*(dep_prop.get(node->get_parent().lock())));
	dep_prop.put(node,dp);
	
	//Visit normally
	visit(node->get_children());
}

//UNARY
void assign_trimmer::visit_sim(std::shared_ptr<ast_sim> node){
	(void) node;
	assert(false); //should NOT be visited
}

void assign_trimmer::visit_prop(std::shared_ptr<ast_prop> node){
	(void) node;
	assert(false); //should NOT be visited
}

//BIN
void assign_trimmer::visit_add(std::shared_ptr<ast_add> node){
	(void) node;
	assert(false); //should NOT be visited
}

void assign_trimmer::visit_sub(std::shared_ptr<ast_sub> node){
	(void) node;
	assert(false); //should NOT be visited
}

void assign_trimmer::visit_mult(std::shared_ptr<ast_mult> node){
	(void) node;
	assert(false); //should NOT be visited
}

void assign_trimmer::visit_div(std::shared_ptr<ast_div> node){
	(void) node;
	assert(false); //should NOT be visited
}

void assign_trimmer::visit_shl(std::shared_ptr<ast_shl> node){
	(void) node;
	assert(false); //should NOT be visited
}

void assign_trimmer::visit_shr(std::shared_ptr<ast_shr> node){
	(void) node;
	assert(false); //should NOT be visited
}

void assign_trimmer::visit_rtl(std::shared_ptr<ast_rtl> node){
	(void) node;
	assert(false); //should NOT be visited
}

void assign_trimmer::visit_rtr(std::shared_ptr<ast_rtr> node){
	(void) node;
	assert(false); //should NOT be visited
}

void assign_trimmer::visit_and(std::shared_ptr<ast_and> node){
	(void) node;
	assert(false); //should NOT be visited
}

void assign_trimmer::visit_or(std::shared_ptr<ast_or> node){
	(void) node;
	assert(false); //should NOT be visited
}

void assign_trimmer::visit_xor(std::shared_ptr<ast_xor> node){
	(void) node;
	assert(false); //should NOT be visited
}

void assign_trimmer::visit_not(std::shared_ptr<ast_not> node){
	(void) node;
	assert(false); //should NOT be visited
}

void assign_trimmer::visit_gap(std::shared_ptr<ast_gap> node){
	(void) node;
	assert(false); //should NOT be visited
}

void assign_trimmer::visit_comp(std::shared_ptr<ast_comp> node){
	(void) node;
	assert(false); //should NOT be visited
}

void assign_trimmer::visit_mod(std::shared_ptr<ast_mod> node){
	(void) node;
	assert(false); //should NOT be visited
}
