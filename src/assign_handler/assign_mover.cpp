#include "assign_mover.h"
#include <memory>

//TODO actually trim the tree

void assign_mover::fix_properties(std::shared_ptr<ast> src,std::shared_ptr<ast> dst){
	//Recursive solve
	
	//Solve this node
	var_id.put(dst,*var_id.get(src));
	//pulse_id.put(dst,*pulse_id.get(src)); //Unused since no cblock short inst are valid
	
	std::vector<std::shared_ptr<ast>> src_childs(src->get_children());
	std::vector<std::shared_ptr<ast>> dst_childs(dst->get_children());
	for(auto src_it=src_childs.begin(),dst_it=dst_childs.begin();
		src_it!=src_childs.end()&&dst_it!=dst_childs.end();
		src_it++,dst_it++){
		fix_properties(*src_it,*dst_it);
	}
}

void assign_mover::rm_properties(std::shared_ptr<ast> target){ 
	//Recursive solve
	
	//Solve this node
	assert(var_id.erase(target));
	//assert(pulse_id.erase(target)); //Unused since no cblock short inst are valid
	
	std::vector<std::shared_ptr<ast>> src_childs(target->get_children());
	for(auto src_it=src_childs.begin();
		src_it!=src_childs.end();src_it++){
		rm_properties(*src_it);
	}
}

void assign_mover::visit_start(std::shared_ptr<ast_start> node){ 
	//Not relevant
	visit(node->get_children());
}

void assign_mover::visit_include(std::shared_ptr<ast_include> node){ 
	//Not relevant
	(void) node;
	//visit(node->get_children());
}

void assign_mover::visit_cblk_decl(std::shared_ptr<ast_cblk_decl> node){ 
	//Visit only code, in _NORMAL_ order
	//Normal, because i want to replace references for their (past declared) expressions
	visit(node->get_code());
}

void assign_mover::visit_cblk_inst(std::shared_ptr<ast_cblk_inst> node){ 
	//FIXME FIXME
	(void) node;
	//visit(node->get_children());
}

void assign_mover::visit_s_cblk_inst(std::shared_ptr<ast_s_cblk_inst> node){ 
	//FIXME FIXME
	(void) node;
	//visit(node->get_children());
}

void assign_mover::visit_pulse(std::shared_ptr<ast_pulse> node){ 
	//Not relevant
	(void) node;
	//visit(node->get_children());
}

void assign_mover::visit_if(std::shared_ptr<ast_if> node){ 
	//FIXME FIXME
	(void) node;
	//IFs require duplication
}

void assign_mover::visit_for(std::shared_ptr<ast_for> node){ 
	//FIXME FIXME
	(void) node;
	//FORs require duplication
}

void assign_mover::visit_decl(std::shared_ptr<ast_decl> node){ 
	//Skip if there is no assignment involved
	if(node->get_attrib()==nullptr){
		return;
	}

	//Check if this assignment is for the default value of a stream
	std::shared_ptr<ast> ins(node->get_var()->get_instant());
	if(ins!=nullptr){ //Has time index, check it
		std::shared_ptr<ast_num> num(std::static_pointer_cast<ast_num>(ins));
		if(num->get_int()<0){ //the time index is valid, count 
			return; //Skip!
		}
	}

	unsigned int this_var(*var_id.get(node->get_var()));

	//Add this var to assign_var
	assign_var.put(node,this_var);

	//Traverse the rest of the declaration
	visit(node->get_attrib());

	//If last branch detached, erase property
	if(detached[this_var]==true){
		rm_properties(assign[this_var]);
	}

	//Add this assignment to the vector
	assign[this_var]=node->get_attrib();

	//Set as attached
	detached[this_var]=false;
}

void assign_mover::visit_assign(std::shared_ptr<ast_assign> node){ 
	unsigned int this_var(*var_id.get(node->get_var()));
	std::weak_ptr<ast> wnode(node);

	//Check if this assignment is for the default value of a stream
	std::shared_ptr<ast> ins(node->get_var()->get_instant());
	if(ins!=nullptr){ //Has time index, check it
		std::shared_ptr<ast_num> num(std::static_pointer_cast<ast_num>(ins));
		if(num->get_int()<0){ //the time index is valid, count 
			return; //Skip!
		}
	}

	//Add this var to assign_var
	assign_var.put(node,this_var);

	//Fix assignment if the attribution is not a EQUAL (=)
	std::shared_ptr<ast_bin_arith> new_node;
	std::shared_ptr<ast_cast> new_cast;

	//Create cast node
	new_cast=std::make_shared<ast_cast>(wnode,vcscope->get()[this_var]->type);
	std::weak_ptr<ast> wnew_cast(new_cast);
	switch(node->get_assign_op().get_op()){
		//Create arith node
		case assign_op::EQUAL:
			new_node=nullptr;
			break;
		case assign_op::ADDEQ:
			new_node=std::make_shared<ast_add>(wnew_cast);
			break;
		case assign_op::SUBEQ:
			new_node=std::make_shared<ast_sub>(wnew_cast);
			break;
		case assign_op::DIVEQ:
			new_node=std::make_shared<ast_div>(wnew_cast);
			break;
		case assign_op::MULEQ:
			new_node=std::make_shared<ast_mult>(wnew_cast);
			break;
		case assign_op::MODEQ:
			new_node=std::make_shared<ast_mod>(wnew_cast);
			break;
	}
	
	//Tie the knot of the new node
	if(new_node!=nullptr){
		std::weak_ptr<ast> wnew_node(new_node);
		std::shared_ptr<ast_arith> anew_node(new_node);
		std::shared_ptr<ast_arith> anew_cast(new_cast);

		//Check if still atached
		if(!detached[this_var]){
			detached[this_var]=true;
			//Remove holder of the prev statement
			assign[this_var]->get_parent().lock()->get_parent().lock()->cut_child(assign[this_var]->get_parent().lock());
		}

		//Set left the prev stored assignment
		std::shared_ptr<ast_arith> mrt(std::static_pointer_cast<ast_arith>(assign[this_var]->duplicate()));
		new_node->set_left(mrt);
		fix_properties(assign[this_var],mrt);

		//Set the new_node as the parent
		new_node->get_left()->set_parent(wnew_node);

		//Set right to the current assignment
		new_node->set_right(node->get_attrib());

		//Set current assignment's parent to new_node
		node->get_attrib()->set_parent(wnew_node);
		
		//Set new_cast target node to new_node
		new_cast->set_target(anew_node);

		//Connect node's attrib to new_cast
		node->set_attrib(anew_cast);

		//Change assign_op back to EQUAL
		node->get_assign_op()=assign_op(assign_op::EQUAL);

		//Fix/Propagate var_id to be used on subnodes
		var_id.put(new_cast,*(var_id.get(new_cast->get_parent().lock())));
		var_id.put(new_node,*(var_id.get(new_node->get_parent().lock())));
	}

	//Traverse the rest of the declaration
	visit(node->get_attrib());

	//If last branch detached, erase property
	if(detached[this_var]==true){
		rm_properties(assign[this_var]);
	}

	//Add this assignment to the vector
	assign[this_var]=node->get_attrib();

	//Set as attached
	detached[this_var]=false;
}

void assign_mover::visit_cast(std::shared_ptr<ast_cast> node){ 
	//Propagate assign_var
	unsigned int this_ass_var(*(assign_var.get(node->get_parent().lock())));
	assign_var.put(node,this_ass_var);

	//Check if next target is a ast_id
	if(node->get_target()->get_op()==ast::ID){
		std::weak_ptr<ast> wnode(node);
		std::shared_ptr<ast_id> id_node(std::static_pointer_cast<ast_id>(node->get_target()));		

		if(id_node->get_instant()!=nullptr){ //Has time index, check it
			std::shared_ptr<ast_num> num(std::static_pointer_cast<ast_num>(id_node->get_instant()));
			bool zero_positive(num->get_is_int()?num->get_int()>=0:num->get_double()>=0);

			if(!zero_positive){ //has valid time index, does not count as normal stream
				return;
			}
			//the time index is valid, thus this stream is always read only
			//In other words, it cannot generate cyclic depedencies
		}

		//Get this_var
		unsigned int this_var(*var_id.get(id_node));

		//If this_var is the same as this_ass_var, then replace
		//id_node will become orfan, and hopefully will be reaped
		if(this_ass_var==this_var){
			//Check if still atached
			if(!detached[this_var]){
				detached[this_var]=true;
				//Remove holder of the prev statement
				assign[this_var]->get_parent().lock()->get_parent().lock()->cut_child(assign[this_var]->get_parent().lock());
			}

			//Connect (a duplicate of the) most recent assign of this_ass_var to this
			std::shared_ptr<ast_arith> mrt(std::static_pointer_cast<ast_arith>(assign[this_ass_var]->duplicate()));
			node->set_target(mrt); 
			mrt->set_parent(wnode);
			fix_properties(assign[this_ass_var],mrt);
		}
	}
	else{
		//Visit normally
		visit(node->get_children());
	}
}

void assign_mover::visit_arrinit(std::shared_ptr<ast_arrinit> node){ 
	//FIXME how to handle this
	visit(node->get_children());
}

void assign_mover::visit_num(std::shared_ptr<ast_num> node){ 
	//Not relevant
	(void) node;
	//visit(node->get_children());
}

void assign_mover::visit_id(std::shared_ptr<ast_id> node){ 
	//Get this_ass_var from assign_var
	unsigned int this_ass_var(*(assign_var.get(node->get_parent().lock())));

	//Get this_var
	unsigned int this_var(*var_id.get(node));

	//this_var should NOT be the same as this_ass_var here
	assert(this_ass_var!=this_var);

	//End of the line
	//visit(node->get_children());
}

//Overall bin/unary
void assign_mover::visit_bin(std::shared_ptr<ast_bin_arith> node){
	//Propagate assign_var
	unsigned int this_ass_var(*(assign_var.get(node->get_parent().lock())));
	assign_var.put(node,this_ass_var);

	//Check if next right is a ast_id
	if(node->get_right()->get_op()==ast::ID){
		std::weak_ptr<ast> wnode(node);
		std::shared_ptr<ast_id> id_node(std::static_pointer_cast<ast_id>(node->get_right()));

		if(id_node->get_instant()!=nullptr){ //Has time index, check it
			std::shared_ptr<ast_num> num(std::static_pointer_cast<ast_num>(id_node->get_instant()));
			bool zero_positive(num->get_is_int()?num->get_int()>=0:num->get_double()>=0);

			if(!zero_positive){ //has valid time index, does not count as normal stream
				return;
			}
			//the time index is valid, thus this stream is always read only
			//In other words, it cannot generate cyclic depedencies
		}

		//Get this_var
		unsigned int this_var(*var_id.get(id_node));

		//If this_var is the same as this_ass_var, then replace
		//id_node will become orfan, and hopefully will be reaped
		if(this_ass_var==this_var){
			//Check if still atached
			if(!detached[this_var]){
				detached[this_var]=true;
				//Remove holder of the prev statement
				assign[this_var]->get_parent().lock()->get_parent().lock()->cut_child(assign[this_var]->get_parent().lock());
			}
			
			//Connect (a duplicate of the) most recent assign of this_ass_var to this
			std::shared_ptr<ast_arith> mrt(std::static_pointer_cast<ast_arith>(assign[this_ass_var]->duplicate()));
			node->set_right(mrt); 
			mrt->set_parent(wnode);
			fix_properties(assign[this_ass_var],mrt);
		}
	}
	else{
		//Visit normally
		visit(node->get_right());
	}

	//Check if next left is a ast_id
	if(node->get_left()->get_op()==ast::ID){
		std::weak_ptr<ast> wnode(node);
		std::shared_ptr<ast_id> id_node(std::static_pointer_cast<ast_id>(node->get_left()));

		if(id_node->get_instant()!=nullptr){ //Has time index, check it
			std::shared_ptr<ast_num> num(std::static_pointer_cast<ast_num>(id_node->get_instant()));
			bool zero_positive(num->get_is_int()?num->get_int()>=0:num->get_double()>=0);

			if(!zero_positive){ //has valid time index, does not count as normal stream
				return;
			}
			//the time index is valid, thus this stream is always read only
			//In other words, it cannot generate cyclic depedencies
		}

		//Get this_var
		unsigned int this_var(*var_id.get(id_node));

		//If this_var is the same as this_ass_var, then replace
		//id_node will become orfan, and hopefully will be reaped
		if(this_ass_var==this_var){
			//Check if still atached
			if(!detached[this_var]){
				detached[this_var]=true;
				//Remove holder of the prev statement
				assign[this_var]->get_parent().lock()->get_parent().lock()->cut_child(assign[this_var]->get_parent().lock());
			}
			
			//Connect (a duplicate of the) most recent assign of this_ass_var to this
			std::shared_ptr<ast_arith> mrt(std::static_pointer_cast<ast_arith>(assign[this_ass_var]->duplicate()));
			node->set_left(mrt);
			mrt->set_parent(wnode);
			fix_properties(assign[this_ass_var],mrt);
		}
	}
	else{
		//Visit normally
		visit(node->get_left());
	}
}

void assign_mover::visit_unary(std::shared_ptr<ast_unary_arith> node){
	//Propagate assign_var
	unsigned int this_ass_var(*(assign_var.get(node->get_parent().lock())));
	assign_var.put(node,this_ass_var);

	//Check if next target is a ast_id
	if(node->get_target()->get_op()==ast::ID){
		std::weak_ptr<ast> wnode(node);
		std::shared_ptr<ast_id> id_node(std::static_pointer_cast<ast_id>(node->get_target()));		

		if(id_node->get_instant()!=nullptr){ //Has time index, check it
			std::shared_ptr<ast_num> num(std::static_pointer_cast<ast_num>(id_node->get_instant()));
			bool zero_positive(num->get_is_int()?num->get_int()>=0:num->get_double()>=0);

			if(!zero_positive){ //has valid time index, does not count as normal stream
				return;
			}
			//the time index is valid, thus this stream is always read only
			//In other words, it cannot generate cyclic depedencies
		}

		//Get this_var
		unsigned int this_var(*var_id.get(id_node));

		//If this_var is the same as this_ass_var, then replace
		//id_node will become orfan, and hopefully will be reaped
		if(this_ass_var==this_var){
			//Check if still atached
			if(!detached[this_var]){
				detached[this_var]=true;
				//Remove holder of the prev statement
				assign[this_var]->get_parent().lock()->get_parent().lock()->cut_child(assign[this_var]->get_parent().lock());
			}
			
			//Connect (a duplicate of the) most recent assign of this_ass_var to this
			std::shared_ptr<ast_arith> mrt(std::static_pointer_cast<ast_arith>(assign[this_ass_var]->duplicate()));
			node->set_target(mrt);
			mrt->set_parent(wnode);
			fix_properties(assign[this_ass_var],mrt);
		}
	}
	else{
		//Visit normally
		visit(node->get_children());
	}
}

//UNARY
void assign_mover::visit_sim(std::shared_ptr<ast_sim> node){
	(void) node;
	assert(false); //should NOT be visited
}

void assign_mover::visit_prop(std::shared_ptr<ast_prop> node){
	(void) node;
	assert(false); //should NOT be visited
}

//BIN
void assign_mover::visit_add(std::shared_ptr<ast_add> node){
	(void) node;
	assert(false); //should NOT be visited
}

void assign_mover::visit_sub(std::shared_ptr<ast_sub> node){
	(void) node;
	assert(false); //should NOT be visited
}

void assign_mover::visit_mult(std::shared_ptr<ast_mult> node){
	(void) node;
	assert(false); //should NOT be visited
}

void assign_mover::visit_div(std::shared_ptr<ast_div> node){
	(void) node;
	assert(false); //should NOT be visited
}

void assign_mover::visit_shl(std::shared_ptr<ast_shl> node){
	(void) node;
	assert(false); //should NOT be visited
}

void assign_mover::visit_shr(std::shared_ptr<ast_shr> node){
	(void) node;
	assert(false); //should NOT be visited
}

void assign_mover::visit_rtl(std::shared_ptr<ast_rtl> node){
	(void) node;
	assert(false); //should NOT be visited
}

void assign_mover::visit_rtr(std::shared_ptr<ast_rtr> node){
	(void) node;
	assert(false); //should NOT be visited
}

void assign_mover::visit_and(std::shared_ptr<ast_and> node){
	(void) node;
	assert(false); //should NOT be visited
}

void assign_mover::visit_or(std::shared_ptr<ast_or> node){
	(void) node;
	assert(false); //should NOT be visited
}

void assign_mover::visit_xor(std::shared_ptr<ast_xor> node){
	(void) node;
	assert(false); //should NOT be visited
}

void assign_mover::visit_not(std::shared_ptr<ast_not> node){
	(void) node;
	assert(false); //should NOT be visited
}

void assign_mover::visit_gap(std::shared_ptr<ast_gap> node){
	(void) node;
	assert(false); //should NOT be visited
}

void assign_mover::visit_comp(std::shared_ptr<ast_comp> node){
	(void) node;
	assert(false); //should NOT be visited
}

void assign_mover::visit_mod(std::shared_ptr<ast_mod> node){
	(void) node;
	assert(false); //should NOT be visited
}
