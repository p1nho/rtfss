#include "const_solver.h"
#include <atomic>
#include <memory>

//FIXME use fixed point

std::shared_ptr<ast_num> const_solver::repl_with_ast_num(std::shared_ptr<ast> curr_node,std::shared_ptr<ast_arith> target_node){
	if(target_node->get_op()==ast::NUM){ //if already a ast_num, don't bother
		return std::static_pointer_cast<ast_num>(target_node);
	}

	std::weak_ptr<ast> wcn(curr_node);
	std::shared_ptr<ast_num> an(std::make_shared<ast_num>(wcn));
	an->set_is_int(*is_int.get(target_node));

	if(an->get_is_int()){
		an->set_int(*int_res.get(target_node));	
	}
	else{
		an->set_double(*double_res.get(target_node));	
	}

	return an;
}

void const_solver::visit_start(std::shared_ptr<ast_start> node){
	//Visit all children
	visit(node->get_children());
}

void const_solver::visit_include(std::shared_ptr<ast_include> node){
	//Useless node in this visitor
	(void) node;
	//visit(node->get_children());
}

void const_solver::visit_cblk_decl(std::shared_ptr<ast_cblk_decl> node){
	//Visit code block
	visit(node->get_code());
}

void const_solver::visit_cblk_inst(std::shared_ptr<ast_cblk_inst> node){
	for(auto cl=node->get_const_list().begin();cl!=node->get_const_list().end();cl++){
		visit(*cl);

		bool cl_const(*is_const.get(*cl));
		if(cl_const==true){ //Replace and give up
			std::shared_ptr<ast_arith> new_num(repl_with_ast_num(node,*cl));
			*cl=new_num;
		}
	}

	for(auto il=node->get_in_list().begin();il!=node->get_in_list().end();il++){
		visit(*il);

		bool il_const(*is_const.get(*il));
		if(il_const==true){ //Replace and give up
			std::shared_ptr<ast_arith> new_num(repl_with_ast_num(node,*il));
			*il=new_num;
		}
	}
}

void const_solver::visit_s_cblk_inst(std::shared_ptr<ast_s_cblk_inst> node){
	//Set node as not const
	bool ic(false);
	is_const.put(node,ic);

	for(auto cl=node->get_const_list().begin();cl!=node->get_const_list().end();cl++){
		visit(*cl);

		bool cl_const(*is_const.get(*cl));
		if(cl_const==true){ //Replace and give up
			std::shared_ptr<ast_arith> new_num(repl_with_ast_num(node,*cl));
			*cl=new_num;
		}
	}

	for(auto il=node->get_in_list().begin();il!=node->get_in_list().end();il++){
		visit(*il);

		bool il_const(*is_const.get(*il));
		if(il_const==true){ //Replace and give up
			std::shared_ptr<ast_arith> new_num(repl_with_ast_num(node,*il));
			*il=new_num;
		}
	}
}

void const_solver::visit_pulse(std::shared_ptr<ast_pulse> node){
	//Useless node in this visitor
	(void) node;
	//visit(node->get_children());
}

void const_solver::visit_if(std::shared_ptr<ast_if> node){
	//Solve logic_expr
	visit(node->get_if_logic());
	bool il_const(*is_const.get(node->get_if_logic()));

	if(il_const==true){ //Replace
		std::shared_ptr<ast_arith> new_num(repl_with_ast_num(node,node->get_if_logic()));
		node->set_if_logic(new_num);
	}

	//Visit if code_block
	visit(node->get_true_code());
	std::vector<std::pair<std::shared_ptr<ast_arith>,std::vector<std::shared_ptr<ast>>>>& elseif(node->get_elseif());
	for(auto ei=elseif.begin();ei!=elseif.end();ei++){
		//Solve logic_expr
		visit(ei->first);
		bool il_const(*is_const.get(ei->first));

		if(il_const==true){ //Replace
			std::shared_ptr<ast_arith> new_num(repl_with_ast_num(node,ei->first));
			*ei=std::make_pair(new_num,ei->second); //Create new tuple
		}

		//Visit elseif code_block
		visit(ei->second);
	}

	//Visit else code_block
	visit(node->get_else_code());
}

void const_solver::visit_for(std::shared_ptr<ast_for> node){
	//Solve lower_bound
	visit(node->get_lower_bound());
	bool lb_const(*is_const.get(node->get_lower_bound()));

	if(lb_const==true){ //Replace
		std::shared_ptr<ast_arith> new_num(repl_with_ast_num(node,node->get_lower_bound()));
		node->set_lower(new_num);
	}

	//Solve upper_bound
	visit(node->get_upper_bound());
	bool ub_const(*is_const.get(node->get_upper_bound()));

	if(ub_const==true){ //Replace
		std::shared_ptr<ast_arith> new_num(repl_with_ast_num(node,node->get_upper_bound()));
		node->set_upper(new_num);
	}

	//Solve increment
	visit(node->get_increment());
	bool inc_const(*is_const.get(node->get_increment()));

	if(inc_const==true){ //Replace
		std::shared_ptr<ast_arith> new_num(repl_with_ast_num(node,node->get_increment()));
		node->set_increment(new_num);
	}

	//Visit for code block
	visit(node->get_code_block());
}

void const_solver::visit_decl(std::shared_ptr<ast_decl> node){
	//Visit var to solve
	visit(node->get_var());

	if(node->get_attrib()!=nullptr){
		visit(node->get_attrib());

		//Solve attrib
		bool attrib_const(*is_const.get(node->get_attrib()));

		if(attrib_const==true){ //Replace
			std::shared_ptr<ast_arith> new_num(repl_with_ast_num(node,node->get_attrib()));
			node->set_attrib(new_num);
		}
	}
}

void const_solver::visit_assign(std::shared_ptr<ast_assign> node){
	//Visit var to solve
	visit(node->get_var());

	visit(node->get_attrib());

	//Solve attrib
	bool attrib_const(*is_const.get(node->get_attrib()));

	if(attrib_const==true){ //Replace
		std::shared_ptr<ast_arith> new_num(repl_with_ast_num(node,node->get_attrib()));
		node->set_attrib(new_num);
	}
}

void const_solver::visit_cast(std::shared_ptr<ast_cast> node){
	//Visit children
	visit(node->get_children());

	//Dont solve, leave it to further ahead
	bool target_const(*is_const.get(node->get_target()));

	//Set node as not const
	bool ic(false);
	is_const.put(node,ic);

	if(target_const==true){ //Replace and give up
		std::shared_ptr<ast_arith> new_num(repl_with_ast_num(node,node->get_target()));
		node->set_target(new_num);
	}
}

void const_solver::visit_arrinit(std::shared_ptr<ast_arrinit> node){
	for(auto elem=node->get_vals().begin();elem!=node->get_vals().end();elem++){
		//Solve elem
		visit(*elem);

		bool target_const(*is_const.get(*elem));
		if(target_const==true){ //Replace and give up
			std::shared_ptr<ast_arith> new_num(repl_with_ast_num(node,*elem));
			*elem=new_num;
		}
	}

	//Set node as not const
	bool ic(false);
	is_const.put(node,ic);
}

void const_solver::visit_num(std::shared_ptr<ast_num> node){
	//Pass info to all ast_property 
	bool ic(true);
	is_const.put(node,ic);

	bool ii(node->get_is_int());
	is_int.put(node,ii);

	if(ii){
		int i(node->get_int());
		int_res.put(node,i);
	}
	else{
		double d(node->get_double());
		double_res.put(node,d);
	}

	//Terminal node
	//visit(node->get_children());
}

void const_solver::visit_id(std::shared_ptr<ast_id> node){
	//Set node as not const
	bool ic(false);
	is_const.put(node,ic);

	//Solve index
	if(node->get_index()!=nullptr){
		visit(node->get_index());
		bool index_const(*is_const.get(node->get_index()));

		if(index_const==true){ //Replace and give up
			std::shared_ptr<ast_arith> new_num(repl_with_ast_num(node,node->get_index()));
			node->set_index(new_num);
		}
	}

	//Solve instant
	if(node->get_instant()!=nullptr){
		visit(node->get_instant());
		bool instant_const(*is_const.get(node->get_instant()));

		if(instant_const==true){ //Replace and give up
			std::shared_ptr<ast_arith> new_num(repl_with_ast_num(node,node->get_instant()));
			node->set_instant(new_num);
		}
	}	
}


//Binary/Unary
void const_solver::visit_add(std::shared_ptr<ast_add> node){
	//Visit children
	visit(node->get_children());

	bool left_const(*is_const.get(node->get_left()));
	bool right_const(*is_const.get(node->get_right()));

	if(left_const==false || right_const==false){ //This node cant be solved
		//Set node as not const
		bool ic(false);
		is_const.put(node,ic);

		if(left_const==true){ //Replace and give up
			std::shared_ptr<ast_arith> new_num(repl_with_ast_num(node,node->get_left()));
			node->set_left(new_num);
		}
		else if(right_const==true){ //Replace and give up
			std::shared_ptr<ast_arith> new_num(repl_with_ast_num(node,node->get_right()));
			node->set_right(new_num);
		}
	}
	else{ //Can be solved in compile-time
		//Calculate and add to ast_property 
		bool ic(true);
		is_const.put(node,ic);

		bool left_is_int(*is_int.get(node->get_left()));
		bool right_is_int(*is_int.get(node->get_right()));

		if(left_is_int&&right_is_int){ //both ints, result int
			bool ii(true);
			is_int.put(node,ii);

			int res(*int_res.get(node->get_left())+*int_res.get(node->get_right()));
			int_res.put(node,res);
		}
		else if(left_is_int&&!right_is_int){ //left int right double, result double
			bool ii(false);
			is_int.put(node,ii);

			double res(*int_res.get(node->get_left())+*double_res.get(node->get_right()));
			double_res.put(node,res);
		}
		else if(!left_is_int&&right_is_int){ //left double right int, result double
			bool ii(false);
			is_int.put(node,ii);

			double res(*double_res.get(node->get_left())+*int_res.get(node->get_right()));
			double_res.put(node,res);
		}
		else{ //both doubles, result double
			bool ii(false);
			is_int.put(node,ii);

			double res(*double_res.get(node->get_left())+*double_res.get(node->get_right()));
			double_res.put(node,res);
		}
	}
}

void const_solver::visit_sub(std::shared_ptr<ast_sub> node){
	//Visit children
	visit(node->get_children());

	bool left_const(*is_const.get(node->get_left()));
	bool right_const(*is_const.get(node->get_right()));

	if(left_const==false || right_const==false){ //This node cant be solved
		//Set node as not const
		bool ic(false);
		is_const.put(node,ic);

		if(left_const==true){ //Replace and give up
			std::shared_ptr<ast_arith> new_num(repl_with_ast_num(node,node->get_left()));
			node->set_left(new_num);
		}
		else if(right_const==true){ //Replace and give up
			std::shared_ptr<ast_arith> new_num(repl_with_ast_num(node,node->get_right()));
			node->set_right(new_num);
		}
	}
	else{ //Can be solved in compile-time
		//Calculate and add to ast_property 
		bool ic(true);
		is_const.put(node,ic);

		bool left_is_int(*is_int.get(node->get_left()));
		bool right_is_int(*is_int.get(node->get_right()));

		if(left_is_int&&right_is_int){ //both ints, result int
			bool ii(true);
			is_int.put(node,ii);

			int res(*int_res.get(node->get_left())-*int_res.get(node->get_right()));
			int_res.put(node,res);
		}
		else if(left_is_int&&!right_is_int){ //left int right double, result double
			bool ii(false);
			is_int.put(node,ii);

			double res(*int_res.get(node->get_left())-*double_res.get(node->get_right()));
			double_res.put(node,res);
		}
		else if(!left_is_int&&right_is_int){ //left double right int, result double
			bool ii(false);
			is_int.put(node,ii);

			double res(*double_res.get(node->get_left())-*int_res.get(node->get_right()));
			double_res.put(node,res);
		}
		else{ //both doubles, result double
			bool ii(false);
			is_int.put(node,ii);

			double res(*double_res.get(node->get_left())-*double_res.get(node->get_right()));
			double_res.put(node,res);
		}
	}
}

void const_solver::visit_sim(std::shared_ptr<ast_sim> node){
	//Visit children
	visit(node->get_children());

	bool target_const(*is_const.get(node->get_target()));

	if(target_const==false){ //This node cant be solved
		//Set node as not const
		bool ic(false);
		is_const.put(node,ic);
	}
	else{ //Can be solved in compile-time
		//Calculate and add to ast_property 
		bool ic(true);
		is_const.put(node,ic);

		bool target_is_int(*is_int.get(node->get_target()));
		is_int.put(node,target_is_int);

		if(target_is_int){ //target ints, result int
			int res(-*int_res.get(node->get_target()));
			int_res.put(node,res);
		}
		else{
			double res(-*double_res.get(node->get_target()));
			double_res.put(node,res);
		}
	}
}

void const_solver::visit_mult(std::shared_ptr<ast_mult> node){
	//Visit children
	visit(node->get_children());

	bool left_const(*is_const.get(node->get_left()));
	bool right_const(*is_const.get(node->get_right()));

	if(left_const==false || right_const==false){ //This node cant be solved
		//Set node as not const
		bool ic(false);
		is_const.put(node,ic);

		if(left_const==true){ //Replace and give up
			std::shared_ptr<ast_arith> new_num(repl_with_ast_num(node,node->get_left()));
			node->set_left(new_num);
		}
		else if(right_const==true){ //Replace and give up
			std::shared_ptr<ast_arith> new_num(repl_with_ast_num(node,node->get_right()));
			node->set_right(new_num);
		}
	}
	else{ //Can be solved in compile-time
		//Calculate and add to ast_property 
		bool ic(true);
		is_const.put(node,ic);

		bool left_is_int(*is_int.get(node->get_left()));
		bool right_is_int(*is_int.get(node->get_right()));

		if(left_is_int&&right_is_int){ //both ints, result int
			bool ii(true);
			is_int.put(node,ii);

			int res(*int_res.get(node->get_left())*(*int_res.get(node->get_right())));
			int_res.put(node,res);
		}
		else if(left_is_int&&!right_is_int){ //left int right double, result double
			bool ii(false);
			is_int.put(node,ii);

			double res(*int_res.get(node->get_left())*(*double_res.get(node->get_right())));
			double_res.put(node,res);
		}
		else if(!left_is_int&&right_is_int){ //left double right int, result double
			bool ii(false);
			is_int.put(node,ii);

			double res(*double_res.get(node->get_left())*(*int_res.get(node->get_right())));
			double_res.put(node,res);
		}
		else{ //both doubles, result double
			bool ii(false);
			is_int.put(node,ii);

			double res(*double_res.get(node->get_left())*(*double_res.get(node->get_right())));
			double_res.put(node,res);
		}
	}
}

void const_solver::visit_div(std::shared_ptr<ast_div> node){
	//Visit children
	visit(node->get_children());

	bool left_const(*is_const.get(node->get_left()));
	bool right_const(*is_const.get(node->get_right()));

	if(left_const==false || right_const==false){ //This node cant be solved
		//Set node as not const
		bool ic(false);
		is_const.put(node,ic);

		if(left_const==true){ //Replace and give up
			std::shared_ptr<ast_arith> new_num(repl_with_ast_num(node,node->get_left()));
			node->set_left(new_num);
		}
		else if(right_const==true){ //Replace and give up
			std::shared_ptr<ast_arith> new_num(repl_with_ast_num(node,node->get_right()));
			node->set_right(new_num);
		}
	}
	else{ //Can be solved in compile-time
		//Calculate and add to ast_property 
		bool ic(true);
		is_const.put(node,ic);

		bool left_is_int(*is_int.get(node->get_left()));
		bool right_is_int(*is_int.get(node->get_right()));

		if(left_is_int&&right_is_int){ //both ints, result int
			bool ii(true);
			is_int.put(node,ii);

			int res(*int_res.get(node->get_left())/(*int_res.get(node->get_right())));
			int_res.put(node,res);
		}
		else if(left_is_int&&!right_is_int){ //left int right double, result double
			bool ii(false);
			is_int.put(node,ii);

			double res(*int_res.get(node->get_left())/(*double_res.get(node->get_right())));
			double_res.put(node,res);
		}
		else if(!left_is_int&&right_is_int){ //left double right int, result double
			bool ii(false);
			is_int.put(node,ii);

			double res(*double_res.get(node->get_left())/(*int_res.get(node->get_right())));
			double_res.put(node,res);
		}
		else{ //both doubles, result double
			bool ii(false);
			is_int.put(node,ii);

			double res(*double_res.get(node->get_left())/(*double_res.get(node->get_right())));
			double_res.put(node,res);
		}
	}
}

void const_solver::visit_shl(std::shared_ptr<ast_shl> node){
	//Visit children
	visit(node->get_children());

	//Dont solve, leave it to further ahead
	bool left_const(*is_const.get(node->get_left()));
	bool right_const(*is_const.get(node->get_right()));

	//Set node as not const
	bool ic(false);
	is_const.put(node,ic);

	if(left_const==true){ //Replace and give up
		std::shared_ptr<ast_arith> new_num(repl_with_ast_num(node,node->get_left()));
		node->set_left(new_num);
	}
	if(right_const==true){ //Replace and give up
		std::shared_ptr<ast_arith> new_num(repl_with_ast_num(node,node->get_right()));
		node->set_right(new_num);
	}
}

void const_solver::visit_shr(std::shared_ptr<ast_shr> node){
	//Visit children
	visit(node->get_children());

	//Dont solve, leave it to further ahead
	bool left_const(*is_const.get(node->get_left()));
	bool right_const(*is_const.get(node->get_right()));

	//Set node as not const
	bool ic(false);
	is_const.put(node,ic);

	if(left_const==true){ //Replace and give up
		std::shared_ptr<ast_arith> new_num(repl_with_ast_num(node,node->get_left()));
		node->set_left(new_num);
	}
	if(right_const==true){ //Replace and give up
		std::shared_ptr<ast_arith> new_num(repl_with_ast_num(node,node->get_right()));
		node->set_right(new_num);
	}
}

void const_solver::visit_rtl(std::shared_ptr<ast_rtl> node){
	//Visit children
	visit(node->get_children());

	//Dont solve, leave it to further ahead
	bool left_const(*is_const.get(node->get_left()));
	bool right_const(*is_const.get(node->get_right()));

	//Set node as not const
	bool ic(false);
	is_const.put(node,ic);

	if(left_const==true){ //Replace and give up
		std::shared_ptr<ast_arith> new_num(repl_with_ast_num(node,node->get_left()));
		node->set_left(new_num);
	}
	if(right_const==true){ //Replace and give up
		std::shared_ptr<ast_arith> new_num(repl_with_ast_num(node,node->get_right()));
		node->set_right(new_num);
	}
}

void const_solver::visit_rtr(std::shared_ptr<ast_rtr> node){
	//Visit children
	visit(node->get_children());

	//Dont solve, leave it to further ahead
	bool left_const(*is_const.get(node->get_left()));
	bool right_const(*is_const.get(node->get_right()));

	//Set node as not const
	bool ic(false);
	is_const.put(node,ic);

	if(left_const==true){ //Replace and give up
		std::shared_ptr<ast_arith> new_num(repl_with_ast_num(node,node->get_left()));
		node->set_left(new_num);
	}
	if(right_const==true){ //Replace and give up
		std::shared_ptr<ast_arith> new_num(repl_with_ast_num(node,node->get_right()));
		node->set_right(new_num);
	}
}

void const_solver::visit_and(std::shared_ptr<ast_and> node){
	//Only solves when both are int 

	//Visit children
	visit(node->get_children());

	bool left_const(*is_const.get(node->get_left()));
	bool right_const(*is_const.get(node->get_right()));

	if(left_const&&right_const){
		bool left_is_int(*is_int.get(node->get_left()));
		bool right_is_int(*is_int.get(node->get_right()));

		//Can only solve if both are int
		if(left_is_int&&right_is_int){
			bool ic(true);
			is_const.put(node,ic);

			bool ii(true);
			is_int.put(node,ii);

			int res(*int_res.get(node->get_left())&(*int_res.get(node->get_right())));
			int_res.put(node,res);
			
			return;
		}
	}

	//Dont solve, leave it to further ahead
	
	//Set node as not const
	bool ic(false);
	is_const.put(node,ic);

	if(left_const==true){ //Replace and give up
		std::shared_ptr<ast_arith> new_num(repl_with_ast_num(node,node->get_left()));
		node->set_left(new_num);
	}
	if(right_const==true){ //Replace and give up
		std::shared_ptr<ast_arith> new_num(repl_with_ast_num(node,node->get_right()));
		node->set_right(new_num);
	}
}

void const_solver::visit_or(std::shared_ptr<ast_or> node){
	//Only solves when both are int 

	//Visit children
	visit(node->get_children());

	bool left_const(*is_const.get(node->get_left()));
	bool right_const(*is_const.get(node->get_right()));

	if(left_const&&right_const){
		bool left_is_int(*is_int.get(node->get_left()));
		bool right_is_int(*is_int.get(node->get_right()));

		//Can only solve if both are int
		if(left_is_int&&right_is_int){
			bool ic(true);
			is_const.put(node,ic);

			bool ii(true);
			is_int.put(node,ii);

			int res(*int_res.get(node->get_left())|(*int_res.get(node->get_right())));
			int_res.put(node,res);
			
			return;
		}
	}

	//Dont solve, leave it to further ahead
	
	//Set node as not const
	bool ic(false);
	is_const.put(node,ic);

	if(left_const==true){ //Replace and give up
		std::shared_ptr<ast_arith> new_num(repl_with_ast_num(node,node->get_left()));
		node->set_left(new_num);
	}
	if(right_const==true){ //Replace and give up
		std::shared_ptr<ast_arith> new_num(repl_with_ast_num(node,node->get_right()));
		node->set_right(new_num);
	}
}

void const_solver::visit_xor(std::shared_ptr<ast_xor> node){
	//Only solves when both are int 

	//Visit children
	visit(node->get_children());

	bool left_const(*is_const.get(node->get_left()));
	bool right_const(*is_const.get(node->get_right()));

	if(left_const&&right_const){
		bool left_is_int(*is_int.get(node->get_left()));
		bool right_is_int(*is_int.get(node->get_right()));

		//Can only solve if both are int
		if(left_is_int&&right_is_int){
			bool ic(true);
			is_const.put(node,ic);

			bool ii(true);
			is_int.put(node,ii);

			int res(*int_res.get(node->get_left())^(*int_res.get(node->get_right())));
			int_res.put(node,res);
			
			return;
		}
	}

	//Dont solve, leave it to further ahead
	
	//Set node as not const
	bool ic(false);
	is_const.put(node,ic);

	if(left_const==true){ //Replace and give up
		std::shared_ptr<ast_arith> new_num(repl_with_ast_num(node,node->get_left()));
		node->set_left(new_num);
	}
	if(right_const==true){ //Replace and give up
		std::shared_ptr<ast_arith> new_num(repl_with_ast_num(node,node->get_right()));
		node->set_right(new_num);
	}
}

void const_solver::visit_not(std::shared_ptr<ast_not> node){
	//Only solves when both are int 

	//Visit children
	visit(node->get_children());

	bool target_const(*is_const.get(node->get_target()));

	if(target_const){
		bool target_is_int(*is_int.get(node->get_target()));

		//Can only solve if its an int
		if(target_is_int){
			bool ic(true);
			is_const.put(node,ic);

			bool ii(true);
			is_int.put(node,ii);

			int res(~(*int_res.get(node->get_target())));
			int_res.put(node,res);
			
			return;
		}
	}

	//Dont solve, leave it to further ahead
	
	//Set node as not const
	bool ic(false);
	is_const.put(node,ic);

	if(target_const==true){ //Replace and give up
		std::shared_ptr<ast_arith> new_num(repl_with_ast_num(node,node->get_target()));
		node->set_target(new_num);
	}
}

void const_solver::visit_gap(std::shared_ptr<ast_gap> node){
	//Visit children
	visit(node->get_children());

	//Dont solve, leave it to further ahead
	bool left_const(*is_const.get(node->get_left()));
	bool right_const(*is_const.get(node->get_right()));

	//Set node as not const
	bool ic(false);
	is_const.put(node,ic);

	if(left_const==true){ //Replace and give up
		std::shared_ptr<ast_arith> new_num(repl_with_ast_num(node,node->get_left()));
		node->set_left(new_num);
	}
	if(right_const==true){ //Replace and give up
		std::shared_ptr<ast_arith> new_num(repl_with_ast_num(node,node->get_right()));
		node->set_right(new_num);
	}
}

template <class L,class R> bool const_solver::compare(logic_op lop,L left,R right){
	switch(lop.get_op()){
		case logic_op::EQUAL:
			return left==right;
		case logic_op::MORE:
			return left>right;
		case logic_op::LESS:
			return left<right;
		case logic_op::MOREQ:
			return left>=right;
		case logic_op::LESEQ:
			return left<=right;
		case logic_op::DIFF:
			return left!=right;
	}
	assert(false);
	return true;
}

void const_solver::visit_comp(std::shared_ptr<ast_comp> node){
	//Visit children
	visit(node->get_children());

	bool left_const(*is_const.get(node->get_left()));
	bool right_const(*is_const.get(node->get_right()));

	if(left_const==false || right_const==false){ //This node cant be solved
		//Set node as not const
		bool ic(false);
		is_const.put(node,ic);

		if(left_const==true){ //Replace and give up
			std::shared_ptr<ast_arith> new_num(repl_with_ast_num(node,node->get_left()));
			node->set_left(new_num);
		}
		else if(right_const==true){ //Replace and give up
			std::shared_ptr<ast_arith> new_num(repl_with_ast_num(node,node->get_right()));
			node->set_right(new_num);
		}
	}
	else{ //Can be solved in compile-time
		//Calculate and add to ast_property 
		bool ic(true);
		is_const.put(node,ic);

		bool left_is_int(*is_int.get(node->get_left()));
		bool right_is_int(*is_int.get(node->get_right()));

		bool ii(true);
		is_int.put(node,ii); //result always int
		if(left_is_int&&right_is_int){ //both ints
			int res=compare<int,int>(node->get_logic_op(),*int_res.get(node->get_left()),*int_res.get(node->get_right()))?1:0;
			int_res.put(node,res);
		}
		else if(left_is_int&&!right_is_int){ //left int right double
			int res(compare<int,double>(node->get_logic_op(),*int_res.get(node->get_left()),*double_res.get(node->get_right()))?1:0);
			int_res.put(node,res);
		}
		else if(!left_is_int&&right_is_int){ //left double right int
			int res(compare<double,int>(node->get_logic_op(),*double_res.get(node->get_left()),*int_res.get(node->get_right()))?1:0);
			int_res.put(node,res);
		}
		else{ //both doubles
			int res(compare<double,double>(node->get_logic_op(),*double_res.get(node->get_left()),*double_res.get(node->get_right()))?1:0);
			int_res.put(node,res);
		}
	}
}

void const_solver::visit_prop(std::shared_ptr<ast_prop> node){
	//Visit children
	visit(node->get_children());

	//Dont solve, leave it to further ahead
	bool target_const(*is_const.get(node->get_target()));

	//Set node as not const
	bool ic(false);
	is_const.put(node,ic);

	if(target_const==true){ //Replace and give up
		std::shared_ptr<ast_arith> new_num(repl_with_ast_num(node,node->get_target()));
		node->set_target(new_num);
	}
}

void const_solver::visit_mod(std::shared_ptr<ast_mod> node){
	//Visit children
	visit(node->get_children());

	bool left_const(*is_const.get(node->get_left()));
	bool right_const(*is_const.get(node->get_right()));

	if(left_const==false || right_const==false){ //This node cant be solved
		//Set node as not const
		bool ic(false);
		is_const.put(node,ic);

		if(left_const==true){ //Replace and give up
			std::shared_ptr<ast_arith> new_num(repl_with_ast_num(node,node->get_left()));
			node->set_left(new_num);
		}
		else if(right_const==true){ //Replace and give up
			std::shared_ptr<ast_arith> new_num(repl_with_ast_num(node,node->get_right()));
			node->set_right(new_num);
		}
	}
	else{ //Can be solved in compile-time
		//Calculate and add to ast_property 
		bool ic(true);
		is_const.put(node,ic);

		bool left_is_int(*is_int.get(node->get_left()));
		bool right_is_int(*is_int.get(node->get_right()));

		//Error checking
		if(!left_is_int){ //has to be int
			error_handler::const_mod_not_int(*double_res.get(node->get_left()));
		}
		if(!right_is_int){ //has to be int
			error_handler::const_mod_not_int(*double_res.get(node->get_right()));
		}

		//both ints, result int
		bool ii(true);
		is_int.put(node,ii);

		int res(*int_res.get(node->get_left())%(*int_res.get(node->get_right())));
		int_res.put(node,res);
	}
}


//Generic Binary/Unary Visitors
void const_solver::visit_bin(std::shared_ptr<ast_bin_arith> node){
	//NEVER visited
	(void) node;
	assert(false);
}

void const_solver::visit_unary(std::shared_ptr<ast_unary_arith> node){
	//NEVER visited
	(void) node;
	assert(false);
}
