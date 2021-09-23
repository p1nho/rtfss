#include "var_solver.h"

void var_solver::sanity_test_vars(std::shared_ptr<var_complete_scope> vcs){
	//Check if all variables have the flag decl true
	int i=0;
	for(auto it=vcs->get().begin();it!=vcs->get().end();it++,i++){
		//std::cerr<<(*it)->to_string()<<std::endl;
		if((*it)->decl==false){ 
			//Var not declared
			error_handler::stream_no_decl((*it)->name);
		}
	}
}

void var_solver::sanity_test_pulses(std::shared_ptr<pulse_complete_scope> pcs){
	//Check if all pulses have the flag decl true
	int i=0;
	for(auto it=pcs->get().begin();it!=pcs->get().end();it++,i++){
		//std::cerr<<(*it)->to_string()<<std::endl;
		if((*it)->decl==false){ 
			//Pulse not declared
			error_handler::pulse_no_decl((*it)->pid.to_string());
		}
	}
}

std::shared_ptr<pulse_property> var_solver::declare_pulse(std::shared_ptr<pulse_scope> ps,std::string& name){
	std::shared_ptr<pulse_property> pulse_var(ps->get_in_scope(ps->create(name)));
	pulse_var->decl=true;

	return pulse_var;
}

std::shared_ptr<pulse_property> var_solver::use_pulse(std::shared_ptr<pulse_scope> ps,std::string& name){
	std::shared_ptr<pulse_property> pulse_var(ps->get_in_scope(name));

	if(pulse_var==nullptr){ //First appearance
		pulse_var=ps->get_in_scope(ps->create(name));
	}

	return pulse_var;
}

void var_solver::visit_start(std::shared_ptr<ast_start> node){
	//"Global" var scope, although (in the current spec) no declaration is allowed here
	std::shared_ptr<var_scope> vs(std::make_shared<var_scope>(true));
	vscopes.put(node,vs);

	//"Global" pulse scope, although (in the current spec) no declaration is allowed here
	//Here is defined "max","masterpulse" and "const" pulses (global/special pulses)
	std::shared_ptr<pulse_scope> ps(std::make_shared<pulse_scope>(true));
	pscopes.put(node,ps);

	//Visit all code
	visit(node->get_children());

	//Run sanity tests
	sanity_test_vars(vs->get_complete());
	sanity_test_pulses(ps->get_complete());

}

void var_solver::visit_include(std::shared_ptr<ast_include> node){
	//Same var scope, reference var_super_scope
	std::shared_ptr<var_scope> var_super_scope(*(vscopes.get(node->get_parent().lock()))); 
	vscopes.put(node,var_super_scope);

	//Same pulse scope, reference pulse_super_scope
	std::shared_ptr<pulse_scope> pulse_super_scope(*(pscopes.get(node->get_parent().lock())));
	pscopes.put(node,pulse_super_scope);

	//Node not relevant
	visit(node->get_children());
}

void var_solver::visit_cblk_decl(std::shared_ptr<ast_cblk_decl> node){
	//New var scope, copy var_scope
	std::shared_ptr<var_scope> var_super_scope(*(vscopes.get(node->get_parent().lock()))); 
	std::shared_ptr<var_scope> vs(std::make_shared<var_scope>(*var_super_scope)); //Copy constructor
	vscopes.put(node,vs);

	//New pulse scope, copy pulse_scope
	std::shared_ptr<pulse_scope> pulse_super_scope(*(pscopes.get(node->get_parent().lock())));
	std::shared_ptr<pulse_scope> ps(std::make_shared<pulse_scope>(*pulse_super_scope)); //Copy constructor
	pscopes.put(node,ps);

	//Set cblk_name
	std::string cblkn(node->get_cblock_name());	
	cblk_name.put(node,cblkn);

	//Handle pulse declaration
	std::string pulse_name(node->get_pid().to_string());
	std::shared_ptr<pulse_property> pulse_p(declare_pulse(ps,pulse_name));
	if(pulse_p->pid.get_pulse_type()==pulse_id::NORMAL){ //Dont overwrite RESERVED consts...
		pulse_p->pk=pulse_property::CBLOCK;
	}
	unsigned int unique_pp(pulse_p->unique_id);
	pulse_id.put(node,unique_pp);

	//Handle const declarations
	std::vector<std::shared_ptr<ast_id>> &consts(node->get_const_list());
	for(auto it=consts.begin();it!=consts.end();it++){
		visit(*it);	//Fill structure
		std::shared_ptr<var_property> var(vs->get_in_scope(*(var_id.get(*it))));	

		if(var->decl==true){ //Redeclaration!
			error_handler::stream_redecl(var->name);
		}
		var->tk=var_property::CONST;
		var->decl=true; //Flag as already declared

		std::string const_name("const");
		var->pid=use_pulse(ps,const_name)->unique_id;
		var->found_pid=true;
	}

	//Handle in declarations
	std::vector<std::shared_ptr<ast_id>> &ins(node->get_in_list());
	for(auto it=ins.begin();it!=ins.end();it++){
		visit(*it);	//Fill structure
		std::shared_ptr<var_property> var(vs->get_in_scope(*(var_id.get(*it))));	

		if(var->decl==true){ //Redeclaration!
			error_handler::stream_redecl(var->name);
		}
		var->tk=var_property::IN;
		var->decl=true;

		var->pid=unique_pp;
		var->found_pid=true;
	}

	//Handle out declarations
	std::vector<std::shared_ptr<ast_id>> &outs(node->get_out_list());
	for(auto it=outs.begin();it!=outs.end();it++){
		visit(*it);	//Fill structure
		std::shared_ptr<var_property> var(vs->get_in_scope(*(var_id.get(*it))));	

		if(var->decl==true){ //Redeclaration!
			error_handler::stream_redecl(var->name);
		}
		var->tk=var_property::OUT;
		var->decl=true;

		var->pid=unique_pp;
		var->found_pid=true;
	}

	//Visit code_block
	visit(node->get_code());
}

void var_solver::visit_cblk_inst(std::shared_ptr<ast_cblk_inst> node){
	//Same scope, reference var_super_scope
	std::shared_ptr<var_scope> var_super_scope(*(vscopes.get(node->get_parent().lock()))); 
	vscopes.put(node,var_super_scope);

	//Same pulse scope, reference pulse_super_scope
	std::shared_ptr<pulse_scope> pulse_super_scope(*(pscopes.get(node->get_parent().lock())));
	pscopes.put(node,pulse_super_scope);

	//Propagate cblk_name
	cblk_name.put(node,*cblk_name.get(node->get_parent().lock()));

	//Handle pulse use
	std::string pulse_name(node->get_pulse_id().to_string());
	unsigned int unique_pp(use_pulse(pulse_super_scope,pulse_name)->unique_id);
	pulse_id.put(node,unique_pp);

	visit(node->get_children());
}

void var_solver::visit_s_cblk_inst(std::shared_ptr<ast_s_cblk_inst> node){
	//Same scope, reference var_super_scope
	std::shared_ptr<var_scope> var_super_scope(*(vscopes.get(node->get_parent().lock()))); 
	vscopes.put(node,var_super_scope);

	//Same pulse scope, reference pulse_super_scope
	std::shared_ptr<pulse_scope> pulse_super_scope(*(pscopes.get(node->get_parent().lock())));
	pscopes.put(node,pulse_super_scope);

	//Propagate cblk_name
	cblk_name.put(node,*cblk_name.get(node->get_parent().lock()));

	//Handle pulse use
	std::string pulse_name(node->get_pulse_id().to_string());
	unsigned int unique_pp(use_pulse(pulse_super_scope,pulse_name)->unique_id);
	pulse_id.put(node,unique_pp);

	visit(node->get_children());
}

void var_solver::visit_pulse(std::shared_ptr<ast_pulse> node){
	//Same scope, reference var_super_scope
	std::shared_ptr<var_scope> var_super_scope(*(vscopes.get(node->get_parent().lock()))); 
	vscopes.put(node,var_super_scope);

	//Same pulse scope, reference pulse_super_scope
	std::shared_ptr<pulse_scope> pulse_super_scope(*(pscopes.get(node->get_parent().lock())));
	pscopes.put(node,pulse_super_scope);

	//Handle pulse declaration
	std::string pulse_name(node->get_pid().to_string());
	std::shared_ptr<pulse_property> pulse_p(declare_pulse(pulse_super_scope,pulse_name));
	pulse_p->pk=pulse_property::REGULAR;
	unsigned int unique_pp(pulse_p->unique_id);
	pulse_id.put(node,unique_pp);

	//Add properties
	pulse_p->pid=node->get_pid();
	pulse_p->ts=node->get_ts();
	pulse_p->pulse_val=node->get_val();

	visit(node->get_children());
}

void var_solver::visit_if(std::shared_ptr<ast_if> node){
	//Hybrid var scoping
	//New scope for each code_block, copy var_scope
	//Change when changes code_block
	std::shared_ptr<var_scope> var_super_scope(*(vscopes.get(node->get_parent().lock()))); 

	//Super scope for if_condition
	vscopes.put(node,var_super_scope);	
	visit(node->get_if_logic());	

	//New pulse scope, copy pulse_scope for if_code_block
	std::shared_ptr<pulse_scope> pulse_super_scope(*(pscopes.get(node->get_parent().lock())));
	std::shared_ptr<pulse_scope> ps(std::make_shared<pulse_scope>(*pulse_super_scope)); //Copy constructor
	pscopes.put(node,ps);

	//Propagate cblk_name
	cblk_name.put(node,*cblk_name.get(node->get_parent().lock()));

	//New scope for if_code_block
	std::shared_ptr<var_scope> if_vs(std::make_shared<var_scope>(*var_super_scope)); //Copy constructor
	vscopes.put(node,if_vs);
	visit(node->get_true_code());	

	//For each elseif
	auto elseif(node->get_elseif());
	for(auto it=elseif.begin();it!=elseif.end();it++){
		
		//Super scope for elsif_condition
		vscopes.put(node,var_super_scope);	
		visit(it->first);	

		//New pulse scope, copy pulse_scope for elsif_code_block
		ps=std::make_shared<pulse_scope>(*pulse_super_scope); //Copy constructor
		pscopes.put(node,ps);

		//New scope for elseif_code_block
		std::shared_ptr<var_scope> elseif_vs(std::make_shared<var_scope>(*var_super_scope)); //Copy constructor
		vscopes.put(node,elseif_vs);
		visit(it->second);	

	}

	//New pulse scope, copy pulse_scope for else_code_block
	ps=std::make_shared<pulse_scope>(*pulse_super_scope); //Copy constructor
	pscopes.put(node,ps);

	//New scope for else_code_block
	std::shared_ptr<var_scope> else_vs(std::make_shared<var_scope>(*var_super_scope)); //Copy constructor
	vscopes.put(node,else_vs);
	visit(node->get_else_code());	

}

void var_solver::visit_for(std::shared_ptr<ast_for> node){
	//Hybrid var scoping
	std::shared_ptr<var_scope> var_super_scope(*(vscopes.get(node->get_parent().lock()))); 

	//Same pulse scope, reference pulse_super_scope
	std::shared_ptr<pulse_scope> pulse_super_scope(*(pscopes.get(node->get_parent().lock())));
	pscopes.put(node,pulse_super_scope);

	//Propagate cblk_name
	cblk_name.put(node,*cblk_name.get(node->get_parent().lock()));

	//Super scope for lower bound, upper bound and increment(if exists)
	vscopes.put(node,var_super_scope);	
	visit(node->get_lower_bound());	
	visit(node->get_upper_bound());	

	if(node->get_increment()!=nullptr){
		visit(node->get_increment());	
	}

	//New scope for for_code_block
	std::shared_ptr<var_scope> for_vs(std::make_shared<var_scope>(*var_super_scope)); //Copy constructor
	vscopes.put(node,for_vs);

	//Add for variable to new scope
	
	//But check first for redeclaration
	if(for_vs->get_in_scope(node->get_for_var())!=nullptr){
		error_handler::stream_redecl(node->get_for_var());
	}

	unsigned int fv(for_vs->create(node->get_for_var()));
	std::shared_ptr<var_property> vpfv(for_vs->get_in_scope(fv));
	vpfv->tk=var_property::FOR_VAR;
	vpfv->decl=true;

	std::string const_name("const");
	vpfv->pid=use_pulse(pulse_super_scope,const_name)->unique_id;
	vpfv->found_pid=true; //handled like a const

	//Visit for code_block
	visit(node->get_code_block());
}

void var_solver::visit_decl(std::shared_ptr<ast_decl> node){
	//Same scope, reference var_super_scope
	std::shared_ptr<var_scope> var_super_scope(*(vscopes.get(node->get_parent().lock()))); 
	vscopes.put(node,var_super_scope);

	//Same pulse scope, reference pulse_super_scope
	std::shared_ptr<pulse_scope> pulse_super_scope(*(pscopes.get(node->get_parent().lock())));
	pscopes.put(node,pulse_super_scope);

	//Propagate cblk_name
	cblk_name.put(node,*cblk_name.get(node->get_parent().lock()));

	//Visit ast_id first
	visit(node->get_var());

	//Complete var
	std::shared_ptr<var_property> var(var_super_scope->get_in_scope(*(var_id.get(node->get_var()))));
	
	if(var->decl){//Check for redeclaration
		error_handler::stream_redecl(var->name);
	}
	var->tk=var_property::REGULAR;
	var->decl=true;

	//Visit attrib if existent
	if(node->get_attrib()!=nullptr){
		//Add id to var_id to be used on subnodes
		var_id.put(node,*(var_id.get(node->get_var())));

		visit(node->get_attrib());
	}
}

void var_solver::visit_assign(std::shared_ptr<ast_assign> node){
	//Same scope, reference var_super_scope
	std::shared_ptr<var_scope> var_super_scope(*(vscopes.get(node->get_parent().lock()))); 
	vscopes.put(node,var_super_scope);

	//Same pulse scope, reference pulse_super_scope
	std::shared_ptr<pulse_scope> pulse_super_scope(*(pscopes.get(node->get_parent().lock())));
	pscopes.put(node,pulse_super_scope);

	//Propagate cblk_name
	cblk_name.put(node,*cblk_name.get(node->get_parent().lock()));

	//Visit ast_id first
	visit(node->get_var());

	//Add id to var_id to be used on subnodes
	var_id.put(node,*(var_id.get(node->get_var())));

	//Visit attrib
	visit(node->get_attrib());
}

void var_solver::visit_cast(std::shared_ptr<ast_cast> node){
	//Same scope, reference var_super_scope
	std::shared_ptr<var_scope> var_super_scope(*(vscopes.get(node->get_parent().lock()))); 
	vscopes.put(node,var_super_scope);

	//Same pulse scope, reference pulse_super_scope
	std::shared_ptr<pulse_scope> pulse_super_scope(*(pscopes.get(node->get_parent().lock())));
	pscopes.put(node,pulse_super_scope);

	//Propagate cblk_name
	cblk_name.put(node,*cblk_name.get(node->get_parent().lock()));

	//Propagate var_id to be used on subnodes
	var_id.put(node,*(var_id.get(node->get_parent().lock())));

	visit(node->get_children());
}

void var_solver::visit_arrinit(std::shared_ptr<ast_arrinit> node){
	//Same scope, reference var_super_scope
	std::shared_ptr<var_scope> var_super_scope(*(vscopes.get(node->get_parent().lock()))); 
	vscopes.put(node,var_super_scope);

	//Same pulse scope, reference pulse_super_scope
	std::shared_ptr<pulse_scope> pulse_super_scope(*(pscopes.get(node->get_parent().lock())));
	pscopes.put(node,pulse_super_scope);

	//Propagate cblk_name
	cblk_name.put(node,*cblk_name.get(node->get_parent().lock()));

	//Propagate var_id to be used on subnodes
	var_id.put(node,*(var_id.get(node->get_parent().lock())));

	visit(node->get_children());
}

void var_solver::visit_num(std::shared_ptr<ast_num> node){
	//Same scope, reference var_super_scope
	std::shared_ptr<var_scope> var_super_scope(*(vscopes.get(node->get_parent().lock()))); 
	vscopes.put(node,var_super_scope);

	//Same pulse scope, reference pulse_super_scope
	std::shared_ptr<pulse_scope> pulse_super_scope(*(pscopes.get(node->get_parent().lock())));
	pscopes.put(node,pulse_super_scope);

	//Propagate cblk_name
	cblk_name.put(node,*cblk_name.get(node->get_parent().lock()));

	//Propagate var_id to be used on subnodes
	var_id.put(node,*(var_id.get(node->get_parent().lock())));

	//Node not relevant
	visit(node->get_children());
}

void var_solver::visit_id(std::shared_ptr<ast_id> node){
	//Same scope, reference var_super_scope
	std::shared_ptr<var_scope> var_super_scope(*(vscopes.get(node->get_parent().lock()))); 
	vscopes.put(node,var_super_scope);

	//Same pulse scope, reference pulse_super_scope
	std::shared_ptr<pulse_scope> pulse_super_scope(*(pscopes.get(node->get_parent().lock())));
	pscopes.put(node,pulse_super_scope);

	//Propagate cblk_name
	cblk_name.put(node,*cblk_name.get(node->get_parent().lock()));

	std::shared_ptr<var_property> this_var(var_super_scope->get_in_scope(node->get_name()));
	if(this_var==nullptr){ //new var on scope
		this_var=var_super_scope->get_in_scope(var_super_scope->create(node->get_name()));

		//Set cblk name
		this_var->cblock_name=*cblk_name.get(node->get_parent().lock());
	}

	//Fill datatype if possible
	if(node->get_datatype().get_datatype()!=datatype::VUNDEF){ //has datatype
		this_var->type=node->get_datatype();
	}

	//Fill pid if possible
	if(node->get_pulse_id().get_pulse_type()!=pulse_id::PUNDEF){ //has valid pulse_id
		std::string pulse_name(node->get_pulse_id().to_string());
		this_var->pid=use_pulse(pulse_super_scope,pulse_name)->unique_id;
	}

	//Populate pulse var_id
	unsigned int unique_id=this_var->unique_id;
	var_id.put(node,unique_id);

	//Visit index if existent
	if(node->get_index()!=nullptr){
		visit(node->get_index());
	}

	//Visit instant if existent
	if(node->get_instant()!=nullptr){
		visit(node->get_instant());
	}
}

//Overall bin/unary
void var_solver::visit_bin(std::shared_ptr<ast_bin_arith> node){
	//Same scope, reference var_super_scope
	std::shared_ptr<var_scope> var_super_scope(*(vscopes.get(node->get_parent().lock()))); 
	vscopes.put(node,var_super_scope);

	//Same pulse scope, reference pulse_super_scope
	std::shared_ptr<pulse_scope> pulse_super_scope(*(pscopes.get(node->get_parent().lock())));
	pscopes.put(node,pulse_super_scope);

	//Propagate cblk_name
	cblk_name.put(node,*cblk_name.get(node->get_parent().lock()));

	//Propagate var_id to be used on subnodes
	var_id.put(node,*(var_id.get(node->get_parent().lock())));

	visit(node->get_children());
}

void var_solver::visit_unary(std::shared_ptr<ast_unary_arith> node){
	//Same scope, reference var_super_scope
	std::shared_ptr<var_scope> var_super_scope(*(vscopes.get(node->get_parent().lock()))); 
	vscopes.put(node,var_super_scope);

	//Same pulse scope, reference pulse_super_scope
	std::shared_ptr<pulse_scope> pulse_super_scope(*(pscopes.get(node->get_parent().lock())));
	pscopes.put(node,pulse_super_scope);

	//Propagate cblk_name
	cblk_name.put(node,*cblk_name.get(node->get_parent().lock()));

	//Propagate var_id to be used on subnodes
	var_id.put(node,*(var_id.get(node->get_parent().lock())));

	visit(node->get_children());
}

//UNARY
void var_solver::visit_sim(std::shared_ptr<ast_sim> node){
	(void) node;
	assert(false); //should NOT be visited
}

void var_solver::visit_prop(std::shared_ptr<ast_prop> node){
	(void) node;
	assert(false); //should NOT be visited
}

//BIN
void var_solver::visit_add(std::shared_ptr<ast_add> node){
	(void) node;
	assert(false); //should NOT be visited
}

void var_solver::visit_sub(std::shared_ptr<ast_sub> node){
	(void) node;
	assert(false); //should NOT be visited
}

void var_solver::visit_mult(std::shared_ptr<ast_mult> node){
	(void) node;
	assert(false); //should NOT be visited
}

void var_solver::visit_div(std::shared_ptr<ast_div> node){
	(void) node;
	assert(false); //should NOT be visited
}

void var_solver::visit_shl(std::shared_ptr<ast_shl> node){
	(void) node;
	assert(false); //should NOT be visited
}

void var_solver::visit_shr(std::shared_ptr<ast_shr> node){
	(void) node;
	assert(false); //should NOT be visited
}

void var_solver::visit_rtl(std::shared_ptr<ast_rtl> node){
	(void) node;
	assert(false); //should NOT be visited
}

void var_solver::visit_rtr(std::shared_ptr<ast_rtr> node){
	(void) node;
	assert(false); //should NOT be visited
}

void var_solver::visit_and(std::shared_ptr<ast_and> node){
	(void) node;
	assert(false); //should NOT be visited
}

void var_solver::visit_or(std::shared_ptr<ast_or> node){
	(void) node;
	assert(false); //should NOT be visited
}

void var_solver::visit_xor(std::shared_ptr<ast_xor> node){
	(void) node;
	assert(false); //should NOT be visited
}

void var_solver::visit_not(std::shared_ptr<ast_not> node){
	(void) node;
	assert(false); //should NOT be visited
}

void var_solver::visit_gap(std::shared_ptr<ast_gap> node){
	(void) node;
	assert(false); //should NOT be visited
}

void var_solver::visit_comp(std::shared_ptr<ast_comp> node){
	(void) node;
	assert(false); //should NOT be visited
}

void var_solver::visit_mod(std::shared_ptr<ast_mod> node){
	(void) node;
	assert(false); //should NOT be visited
}
