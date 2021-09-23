#include "arch_gen.h"
#include <memory>


boost::dynamic_bitset<> arch_gen::ins_fixed_point(std::shared_ptr<ast_num> node,datatype dom_dt,datatype& ret_dt,std::string name){

	unsigned int integ_part;
	unsigned int frac_part;
	datatype::dt_types dtt;

	boost::dynamic_bitset<> fixed_num;	
	if(node->get_is_int()){
		if(dom_dt.get_datatype()==datatype::UNSIGNED){
			integ_part=(unsigned int) std::ceil(std::log2(node->get_int()+1));
			dtt=datatype::UNSIGNED;
		}
		else{
			integ_part=(unsigned int) std::ceil(std::log2(-node->get_int()+1))+1;
			dtt=datatype::SIGNED;
		}

		//Clip if needed
		//Warning check: if not precise representation, raise warning
		if(integ_part>dom_dt.get_integ()){
			warning_handler::ins_num_integ_imprecise_representation(name,node->get_int(),dom_dt);
		}
		integ_part=dom_dt.get_integ();

		frac_part=0;
		fixed_num=boost::dynamic_bitset<>(integ_part,node->get_int());
	}
	else{
		if(dom_dt.get_datatype()==datatype::UNSIGNED){
			integ_part=(unsigned int) std::ceil(std::log2(std::floor(node->get_double()+1)));
			dtt=datatype::UNSIGNED;
		}
		else{
			integ_part=(unsigned int) std::ceil(std::log2(std::floor(-node->get_double()+1)))+1;
			dtt=datatype::SIGNED;
		}

		//Calculate bits of frac, with the limit of the frac of dominant datatype
		double div(node->get_double()),integ;
		bool precise=false;

		div=std::modf(div,&integ);
		for(frac_part=0;frac_part<=dom_dt.get_frac();frac_part++){
			if(div==0){
				precise=true;
			}
			div=std::modf(div*2,&integ);
		}	
		frac_part--;

		//Warning check: if precise is not set, the limit of the representation was reached
		//Warning check: if not precise representation of integer part, raise warning
		//Clip if needed
		if(!precise||integ_part>dom_dt.get_integ()){
			warning_handler::ins_num_frac_imprecise_representation(name,node->get_double(),dom_dt);
		}
		integ_part=dom_dt.get_integ();

		long fixed(node->get_double()*(1<<frac_part)); //shift wanted bits to integ part
		fixed_num=boost::dynamic_bitset<>(integ_part+frac_part,fixed);
	}
	ret_dt=datatype(dtt,integ_part,frac_part);

	return fixed_num;
}

void arch_gen::visit_start(std::shared_ptr<ast_start> node){
	visit(node->get_children());
}

void arch_gen::visit_include(std::shared_ptr<ast_include> node){
	//Do nothing
	(void) node;
}

void arch_gen::visit_cblk_decl(std::shared_ptr<ast_cblk_decl> node){
	//FIXME while this is not implemented, all variables must be used, or frepr unsyncs node ids
	//TODO
	
	//In this stage of the compiler's development, only the main cblock is accepted
	//So, throw an error if we are visiting a cblock not named main
	if(node->get_cblock_name()!="main"){
		error_handler::cblock_not_main(node->get_cblock_name());
	}
	
	visit(node->get_code());
}

void arch_gen::visit_cblk_inst(std::shared_ptr<ast_cblk_inst> node){
	//TODO
	visit(node->get_children());
}

void arch_gen::visit_s_cblk_inst(std::shared_ptr<ast_s_cblk_inst> node){
	//TODO
	visit(node->get_children());
}

void arch_gen::visit_pulse(std::shared_ptr<ast_pulse> node){
	//Do nothing ?
	(void) node;
}

void arch_gen::visit_if(std::shared_ptr<ast_if> node){
	//TODO
	visit(node->get_children());
}

void arch_gen::visit_for(std::shared_ptr<ast_for> node){
	//TODO
	visit(node->get_children());
}

void arch_gen::visit_decl(std::shared_ptr<ast_decl> node){
	//Visit id
	visit(node->get_var());
	std::shared_ptr<arch_node> named_var(*result.get(node->get_var()));

	if(node->get_attrib()!=nullptr){

		//Set dominant datatype
		datatype dt_expected(named_var->get_datatype());
		dominant_dt.put(node,dt_expected);

		//Visit attribution
		std::shared_ptr<ast_arith> at(node->get_attrib());
		visit(at);
		std::shared_ptr<arch_node> tmp_var(*result.get(at));

		//Check if the named_var is a INS
		if(named_var->get_op()==arch_node::INS){
			//It is, then it means that this is a assignment for the default value
			std::shared_ptr<arch_ins> ains(std::static_pointer_cast<arch_ins>(named_var));

			//This can only be accepted, at least at this stage of development,
			//when the attribution is a numerical or a cast ot a numerical
			//Error checking
			
			boost::dynamic_bitset<> target_dbs;
			datatype target_dt;
			if(tmp_var->get_op()==arch_node::CAST){ //Check if its a cast
				std::shared_ptr<arch_cast> ac(std::static_pointer_cast<arch_cast>(tmp_var));
				if(ac->get_target()->get_op()==arch_node::NUM){ 
					//If the cast target is a num, recreate numeric val with the dominant dt equal to the cast

					//Set dominant dt
					datatype dom_dt(ac->get_datatype());	

					//Get ast_num node to reinst
					std::shared_ptr<ast_cast> aac(std::static_pointer_cast<ast_cast>(node->get_attrib()));
					std::shared_ptr<ast_num> aan(std::static_pointer_cast<ast_num>(aac->get_target()));

					target_dbs=ins_fixed_point(aan,dom_dt,target_dt,ains->get_name());

					//Remove const <-> num relation and nodes from graph
					graph.rm_dep(tmp_var,ac->get_target());
					graph.rm_node(tmp_var);
					graph.rm_node(ac->get_target());
				}
			}
			else if(tmp_var->get_op()==arch_node::NUM){ //Check if its a number 
				std::shared_ptr<arch_num> ac(std::static_pointer_cast<arch_num>(tmp_var));

				target_dt=ac->get_datatype();
				target_dbs=ac->get_num();
			}
			//Error checking
			else{
				error_handler::inst_attr_not_const(ains->get_name(),-(int(ains->get_inst())-1));
			}

			//Error checking
			//Check if the sizing is correct
			if(!dt_expected.equal(target_dt)){
				unsigned int orig_var_id(*var_id.get(node->get_var()));
				std::shared_ptr<var_property> var_prop(vcscope->get()[orig_var_id]);
				error_handler::datatype_mismatch(var_prop->name,dt_expected,target_dt);
			}

			//Set default_value of arch_ins
			ains->set_default_value(target_dbs);
		}
		else{
			//Error checking: datatype recieved and the one expected must be the same
			datatype dt_got(tmp_var->get_datatype());
			if(!dt_expected.equal(dt_got)){
				unsigned int orig_var_id(*var_id.get(node->get_var()));
				std::shared_ptr<var_property> var_prop(vcscope->get()[orig_var_id]);
				error_handler::datatype_mismatch(var_prop->name,dt_expected,dt_got);
			}

			//Connect temp var to named var
			graph.add_dep(named_var,tmp_var);
		}
	}
}

void arch_gen::visit_assign(std::shared_ptr<ast_assign> node){
	//Visit id
	visit(node->get_var());
	std::shared_ptr<arch_node> named_var(*result.get(node->get_var()));

	//Set dominant datatype
	datatype dt_expected(named_var->get_datatype());
	dominant_dt.put(node,dt_expected);

	//Visit attribution
	std::shared_ptr<ast_arith> at(node->get_attrib());
	visit(at);
	std::shared_ptr<arch_node> tmp_var(*result.get(at));

	//Check if the named_var is a INS
	if(named_var->get_op()==arch_node::INS){
		//It is, then it means that this is a assignment for the default value
		std::shared_ptr<arch_ins> ains(std::static_pointer_cast<arch_ins>(named_var));

		//This can only be accepted, at least at this stage of development,
		//when the attribution is a numerical or a cast ot a numerical
		//Error checking
		
		boost::dynamic_bitset<> target_dbs;
		datatype target_dt;
		if(tmp_var->get_op()==arch_node::CAST){ //Check if its a cast
			std::shared_ptr<arch_cast> ac(std::static_pointer_cast<arch_cast>(tmp_var));
			if(ac->get_target()->get_op()==arch_node::NUM){ 
				//If the cast target is a num, recreate numeric val with the dominant dt equal to the cast

				//Set dominant dt
				datatype dom_dt(ac->get_datatype());	

				//Get ast_num node to reinst
				std::shared_ptr<ast_cast> aac(std::static_pointer_cast<ast_cast>(node->get_attrib()));
				std::shared_ptr<ast_num> aan(std::static_pointer_cast<ast_num>(aac->get_target()));

				target_dbs=ins_fixed_point(aan,dom_dt,target_dt,ains->get_name());

				//Remove const <-> num relation and nodes from graph
				graph.rm_dep(tmp_var,ac->get_target());
				graph.rm_node(tmp_var);
				graph.rm_node(ac->get_target());
			}
		}
		else if(tmp_var->get_op()==arch_node::NUM){ //Check if its a number 
			std::shared_ptr<arch_num> ac(std::static_pointer_cast<arch_num>(tmp_var));

			target_dt=ac->get_datatype();
			target_dbs=ac->get_num();
		}
		//Error checking
		else{
			error_handler::inst_attr_not_const(ains->get_name(),-(int(ains->get_inst())-1));
		}

		//Error checking
		//Check if the sizing is correct
		if(!dt_expected.equal(target_dt)){
			unsigned int orig_var_id(*var_id.get(node->get_var()));
			std::shared_ptr<var_property> var_prop(vcscope->get()[orig_var_id]);
			error_handler::datatype_mismatch(var_prop->name,dt_expected,target_dt);
		}

		//Set default_value of arch_ins
		ains->set_default_value(target_dbs);
	}
	else{
		//Error checking: datatype recieved and the one expected must be the same
		datatype dt_got(tmp_var->get_datatype());
		if(!dt_expected.equal(dt_got)){
			unsigned int orig_var_id(*var_id.get(node->get_var()));
			std::shared_ptr<var_property> var_prop(vcscope->get()[orig_var_id]);
			error_handler::datatype_mismatch(var_prop->name,dt_expected,dt_got);
		}

		//Connect temp var to named var
		graph.add_dep(named_var,tmp_var);
	}
}

void arch_gen::visit_cast(std::shared_ptr<ast_cast> node){
	//Set dominant datatype
	datatype dt(node->get_datatype());
	dominant_dt.put(node,dt);

	//Visit target
	visit(node->get_target());

	//Get pid and datatype
	unsigned int orig_var_id(*var_id.get(node));
	std::shared_ptr<var_property> var_prop(vcscope->get()[orig_var_id]);
	unsigned int pid(var_prop->pid);

	//Create new node
	std::shared_ptr<arch_cast> this_node(std::make_shared<arch_cast>(var_cnt++,pid,dt));
	std::shared_ptr<arch_node> tn(std::static_pointer_cast<arch_node>(this_node));
	
	//Add depencendies to graph
	std::shared_ptr<arch_node> target(*result.get(node->get_target()));
	this_node->set_target(target);
	graph.add_dep(tn,target);

	//Add this node to result
	result.put(node,tn);
}

void arch_gen::visit_arrinit(std::shared_ptr<ast_arrinit> node){
	//TODO
	//Dont forget to propagate dominant datatype
	visit(node->get_children());
}

void arch_gen::visit_num(std::shared_ptr<ast_num> node){
	//Get pid and datatype
	unsigned int orig_var_id(*var_id.get(node));
	std::shared_ptr<var_property> var_prop(vcscope->get()[orig_var_id]);
	unsigned int pid(var_prop->pid);
	
	//Final node, no need to propagate dominant datatype

	unsigned int integ_part;
	unsigned int frac_part;
	datatype::dt_types dtt;

	//Get dominant datatype of this expression
	datatype *dom_dt(dominant_dt.get(node->get_parent().lock()));

	//Assumes no floating point representation is allowed from this point on
	boost::dynamic_bitset<> fixed_num;	
	if(node->get_is_int()){
		if(node->get_int()>=0){
			integ_part=(unsigned int) std::ceil(std::log2(node->get_int()+1));
			dtt=datatype::UNSIGNED;
		}
		else{
			integ_part=(unsigned int) std::ceil(std::log2(-node->get_int()+1))+1;
			dtt=datatype::SIGNED;
		}

		//Clip if needed
		//Warning check: if not precise representation, raise warning
		if(integ_part>dom_dt->get_integ()){
			warning_handler::num_integ_imprecise_representation(var_prop->name,node->get_int(),*dom_dt);
			integ_part=dom_dt->get_integ();
		}

		frac_part=0;
		fixed_num=boost::dynamic_bitset<>(integ_part,node->get_int());
	}
	else{
		if(node->get_double()>=0){
			integ_part=(unsigned int) std::ceil(std::log2(std::floor(node->get_double()+1)));
			dtt=datatype::UNSIGNED;
		}
		else{
			integ_part=(unsigned int) std::ceil(std::log2(std::floor(-node->get_double()+1)))+1;
			dtt=datatype::SIGNED;
		}

		//Calculate bits of frac, with the limit of the frac of dominant datatype
		double div(node->get_double()),integ;
		bool precise=false;

		div=std::modf(div,&integ);
		for(frac_part=0;frac_part<=dom_dt->get_frac();frac_part++){
			if(div==0){
				precise=true;
				break;
			}
			div=std::modf(div*2,&integ);
		}	

		//Warning check: if precise is not set, the limit of the representation was reached
		//Warning check: if not precise representation of integer part, raise warning
		//Clip if needed
		if(!precise||integ_part>dom_dt->get_integ()){
			warning_handler::num_frac_imprecise_representation(var_prop->name,node->get_double(),*dom_dt);
			integ_part=dom_dt->get_integ();
		}

		long fixed(node->get_double()*(1<<frac_part)); //shift wanted bits to integ part
		fixed_num=boost::dynamic_bitset<>(integ_part+frac_part,fixed);
		dtt=node->get_double()>=0?datatype::UNSIGNED:datatype::SIGNED;
	}
	datatype dt(dtt,integ_part,frac_part);

	//Create node
	std::shared_ptr<arch_num> this_node(std::make_shared<arch_num>(var_cnt++,pid,dt,fixed_num));
	std::shared_ptr<arch_node> tn(std::static_pointer_cast<arch_node>(this_node));

	//Add to result
	result.put(node,tn);
}

void arch_gen::visit_id(std::shared_ptr<ast_id> node){
	//Get pid and datatype
	unsigned int orig_var_id(*var_id.get(node));
	std::shared_ptr<var_property> var_prop(vcscope->get()[orig_var_id]);
	unsigned int pid(var_prop->pid);
	datatype dt(var_prop->type);
	std::string& var_name(var_prop->name);
	std::string& cblock_name(var_prop->cblock_name);
	var_property::var_kind vk(var_prop->tk);

	//Final node, no need to propagate dominant datatype

	//Check if node already generated
	std::shared_ptr<arch_node> this_node(graph.get_node(orig_var_id));
	if(this_node==nullptr){
		this_node=std::static_pointer_cast<arch_node>(std::make_shared<arch_id>(orig_var_id,pid,dt,var_name,cblock_name,vk));
		graph.add_node(this_node);
	}
	
    //Now, check if normal var, or is an instant
    if(node->get_instant()!=nullptr){ //Has time index, check it
        std::shared_ptr<ast_num> num(std::static_pointer_cast<ast_num>(node->get_instant()));

		int iinst(num->get_int());
        if(iinst<0){ //has valid time index, handle like a arch_ins
			unsigned int uinst(-(iinst+1)); //flip polarity and make index-worthy
			std::vector<std::shared_ptr<arch_ins>> &this_inst_hist(inst_history[orig_var_id]);
			
			//Check if the node wanted already exists
			if(this_inst_hist.size()>uinst){
				//Exists. get and add to result and return
				this_node=this_inst_hist[uinst];
			}
			else{
				//Doesnt exist, have to generate chain from the last inst of this stream
				std::shared_ptr<arch_ins> this_inst;

				if(this_inst_hist.size()==0){
					this_inst=std::make_shared<arch_ins>(var_cnt++,pid,dt,var_name,vk,0);
					this_inst->set_target(this_node); //add orig node as target
					graph.add_dep(this_inst,this_node); //add dep to graph
					this_inst_hist.push_back(this_inst); //add to history
				}
				
				while(this_inst_hist.size()<=uinst){
					std::shared_ptr<arch_node> prev_inst(this_inst_hist[this_inst_hist.size()-1]); //Get latest
					this_inst=std::make_shared<arch_ins>(var_cnt++,pid,dt,var_name,vk,this_inst_hist.size());
					this_inst->set_target(prev_inst); //add prev_inst node as target
					graph.add_dep(this_inst,prev_inst); //add dep to graph
					this_inst_hist.push_back(this_inst); //add to history
				}
				this_node=this_inst; //replace this_node with new one
			}
        }
        //else keep going. the time index is not negative, so handle like a arch_node
    }

	//Add to result
	result.put(node,this_node);
}


//Binary/Unary
void arch_gen::visit_add(std::shared_ptr<ast_add> node){
	//Propagate dominant datatype
	dominant_dt.put(node,*dominant_dt.get(node->get_parent().lock()));

	//Visit children
	visit(node->get_children());

	//Get pid
	unsigned int orig_var_id(*var_id.get(node));
	std::shared_ptr<var_property> var_prop(vcscope->get()[orig_var_id]);
	unsigned int pid(var_prop->pid);

	//Get left and right
	std::shared_ptr<arch_node> left(*result.get(node->get_left()));
	std::shared_ptr<arch_node> right(*result.get(node->get_right()));

	datatype dtl(left->get_datatype());
	datatype dtr(right->get_datatype());

	//Calculate final datatype
	datatype::dt_types ddt;
	if(dtl.get_datatype()==datatype::SIGNED || dtr.get_datatype()==datatype::SIGNED){
		ddt=datatype::SIGNED;
	}
	else{
		ddt=datatype::UNSIGNED;
	}
	datatype dt(ddt,std::max(dtl.get_integ(),dtr.get_integ())+1,std::max(dtl.get_frac(),dtr.get_frac()));

	//Create new node
	std::shared_ptr<arch_add> this_node(std::make_shared<arch_add>(var_cnt++,pid,dt));
	std::shared_ptr<arch_node> tn(std::static_pointer_cast<arch_node>(this_node));

	//Add depencendies to graph
	graph.add_dep(tn,left);
	graph.add_dep(tn,right);

	//Add to graph nodes
	this_node->set_left(left);
	this_node->set_right(right);

	//Add this node to result
	result.put(node,tn);
}

void arch_gen::visit_sub(std::shared_ptr<ast_sub> node){
	//Propagate dominant datatype
	dominant_dt.put(node,*dominant_dt.get(node->get_parent().lock()));

	//Visit children
	visit(node->get_children());

	//Get pid
	unsigned int orig_var_id(*var_id.get(node));
	std::shared_ptr<var_property> var_prop(vcscope->get()[orig_var_id]);
	unsigned int pid(var_prop->pid);

	//Get left and right
	std::shared_ptr<arch_node> left(*result.get(node->get_left()));
	std::shared_ptr<arch_node> right(*result.get(node->get_right()));

	datatype dtl(left->get_datatype());
	datatype dtr(right->get_datatype());

	//Calculate final datatype
	datatype::dt_types ddt;
	if(dtl.get_datatype()==datatype::SIGNED || dtr.get_datatype()==datatype::SIGNED){
		ddt=datatype::SIGNED;
	}
	else{
		ddt=datatype::UNSIGNED;
	}
	datatype dt(ddt,std::max(dtl.get_integ(),dtr.get_integ())+1,std::max(dtl.get_frac(),dtr.get_frac()));

	//Create new node
	std::shared_ptr<arch_sub> this_node(std::make_shared<arch_sub>(var_cnt++,pid,dt));
	std::shared_ptr<arch_node> tn(std::static_pointer_cast<arch_node>(this_node));

	//Add depencendies to graph
	graph.add_dep(tn,left);
	graph.add_dep(tn,right);

	//Add to graph nodes
	this_node->set_left(left);
	this_node->set_right(right);

	//Add this node to result
	result.put(node,tn);
}

void arch_gen::visit_sim(std::shared_ptr<ast_sim> node){
	//Propagate dominant datatype
	dominant_dt.put(node,*dominant_dt.get(node->get_parent().lock()));

	//Visit children
	visit(node->get_children());

	//Get pid
	unsigned int orig_var_id(*var_id.get(node));
	std::shared_ptr<var_property> var_prop(vcscope->get()[orig_var_id]);
	unsigned int pid(var_prop->pid);

	//Get target
	std::shared_ptr<arch_node> target(*result.get(node->get_target()));

	datatype dtt(target->get_datatype());

	//Calculate final datatype
	datatype dt(datatype::SIGNED,dtt.get_integ()+1,dtt.get_frac());

	//Create new node
	std::shared_ptr<arch_sim> this_node(std::make_shared<arch_sim>(var_cnt++,pid,dt));
	std::shared_ptr<arch_node> tn(std::static_pointer_cast<arch_node>(this_node));

	//Add depencendies to graph
	graph.add_dep(tn,target);

	//Add to graph nodes
	this_node->set_target(target);

	//Add this node to result
	result.put(node,tn);
}

void arch_gen::visit_mult(std::shared_ptr<ast_mult> node){
	//Propagate dominant datatype
	dominant_dt.put(node,*dominant_dt.get(node->get_parent().lock()));

	//Visit children
	visit(node->get_children());

	//Get pid
	unsigned int orig_var_id(*var_id.get(node));
	std::shared_ptr<var_property> var_prop(vcscope->get()[orig_var_id]);
	unsigned int pid(var_prop->pid);

	//Get left and right
	std::shared_ptr<arch_node> left(*result.get(node->get_left()));
	std::shared_ptr<arch_node> right(*result.get(node->get_right()));

	datatype dtl(left->get_datatype());
	datatype dtr(right->get_datatype());

	//Calculate final datatype
	datatype::dt_types ddt;
	if(dtl.get_datatype()==datatype::SIGNED || dtr.get_datatype()==datatype::SIGNED){
		ddt=datatype::SIGNED;
	}
	else{
		ddt=datatype::UNSIGNED;
	}
	datatype dt(ddt,dtl.get_integ()+dtr.get_integ(),dtl.get_frac()+dtr.get_frac());

	//Create new node
	std::shared_ptr<arch_mult> this_node(std::make_shared<arch_mult>(var_cnt++,pid,dt));
	std::shared_ptr<arch_node> tn(std::static_pointer_cast<arch_node>(this_node));

	//Add depencendies to graph
	graph.add_dep(tn,left);
	graph.add_dep(tn,right);

	//Add to graph nodes
	this_node->set_left(left);
	this_node->set_right(right);

	//Add this node to result
	result.put(node,tn);
}

void arch_gen::visit_div(std::shared_ptr<ast_div> node){
	//Propagate dominant datatype
	dominant_dt.put(node,*dominant_dt.get(node->get_parent().lock()));

	//Visit children
	visit(node->get_children());

	//Get pid
	unsigned int orig_var_id(*var_id.get(node));
	std::shared_ptr<var_property> var_prop(vcscope->get()[orig_var_id]);
	unsigned int pid(var_prop->pid);

	//Get left and right
	std::shared_ptr<arch_node> left(*result.get(node->get_left()));
	std::shared_ptr<arch_node> right(*result.get(node->get_right()));

	datatype dtl(left->get_datatype());
	datatype dtr(right->get_datatype());

	//Calculate final datatype
	datatype::dt_types ddt;
	if(dtl.get_datatype()==datatype::SIGNED || dtr.get_datatype()==datatype::SIGNED){
		ddt=datatype::SIGNED;
	}
	else{
		ddt=datatype::UNSIGNED;
	}
	datatype dt(ddt,dtl.get_integ()+dtr.get_frac(),dtl.get_frac()+dtr.get_integ());

	//Create new node
	std::shared_ptr<arch_div> this_node(std::make_shared<arch_div>(var_cnt++,pid,dt));
	std::shared_ptr<arch_node> tn(std::static_pointer_cast<arch_node>(this_node));

	//Add depencendies to graph
	graph.add_dep(tn,left);
	graph.add_dep(tn,right);

	//Add to graph nodes
	this_node->set_left(left);
	this_node->set_right(right);

	//Add this node to result
	result.put(node,tn);
}

void arch_gen::visit_shl(std::shared_ptr<ast_shl> node){
	//Propagate dominant datatype
	dominant_dt.put(node,*dominant_dt.get(node->get_parent().lock()));

	//Visit children
	visit(node->get_children());

	//Get pid
	unsigned int orig_var_id(*var_id.get(node));
	std::shared_ptr<var_property> var_prop(vcscope->get()[orig_var_id]);
	unsigned int pid(var_prop->pid);

	//Get left and right
	std::shared_ptr<arch_node> left(*result.get(node->get_left()));
	std::shared_ptr<arch_node> right(*result.get(node->get_right()));

	//Error checking, right must be arch_num
	if(right->get_op()!=arch_node::NUM){ //0->const
		error_handler::shl_right_not_const(var_prop->name);
	}
	
	//Error checking, right must be integer (not frac)
	if(right->get_datatype().get_frac()!=0){
		error_handler::shl_frac_shift(var_prop->name);
	}
	
	//Error checking, right must be unsigned
	if(right->get_datatype().get_datatype()!=datatype::UNSIGNED){
		error_handler::shl_not_unsigned(var_prop->name);
	}

	//Calculate final datatype
	unsigned long shamt(std::static_pointer_cast<arch_num>(right)->get_num().to_ulong());
	datatype dtl(left->get_datatype());
	datatype dt(dtl.get_datatype(),
				dtl.get_integ()+shamt,
				shamt<dtl.get_frac()?dtl.get_frac()-shamt:0);

	//Create new node
	std::shared_ptr<arch_shl> this_node(std::make_shared<arch_shl>(var_cnt++,pid,dt));
	std::shared_ptr<arch_node> tn(std::static_pointer_cast<arch_node>(this_node));

	//Add depencendies to graph
	graph.add_dep(tn,left);
	graph.add_dep(tn,right);

	//Add to graph nodes
	this_node->set_left(left);
	this_node->set_right(right);

	//Add this node to result
	result.put(node,tn);
}

void arch_gen::visit_shr(std::shared_ptr<ast_shr> node){
	//Propagate dominant datatype
	dominant_dt.put(node,*dominant_dt.get(node->get_parent().lock()));

	//Visit children
	visit(node->get_children());

	//Get pid
	unsigned int orig_var_id(*var_id.get(node));
	std::shared_ptr<var_property> var_prop(vcscope->get()[orig_var_id]);
	unsigned int pid(var_prop->pid);

	//Get left and right
	std::shared_ptr<arch_node> left(*result.get(node->get_left()));
	std::shared_ptr<arch_node> right(*result.get(node->get_right()));

	//Error checking, right must be arch_num
	if(right->get_op()!=arch_node::NUM){ //0->const
		error_handler::shr_right_not_const(var_prop->name);
	}
	
	//Error checking, right must be integer (not frac)
	if(right->get_datatype().get_frac()!=0){
		error_handler::shr_frac_shift(var_prop->name);
	}
	
	//Error checking, right must be unsigned
	if(right->get_datatype().get_datatype()!=datatype::UNSIGNED){
		error_handler::shr_not_unsigned(var_prop->name);
	}

	//Calculate final datatype
	unsigned long shamt(std::static_pointer_cast<arch_num>(right)->get_num().to_ulong());
	datatype dtl(left->get_datatype());
	datatype dt(dtl.get_datatype(),
				shamt<dtl.get_integ()?dtl.get_integ()-shamt:0,
				dtl.get_frac()+shamt);

	//Create new node
	std::shared_ptr<arch_shr> this_node(std::make_shared<arch_shr>(var_cnt++,pid,dt));
	std::shared_ptr<arch_node> tn(std::static_pointer_cast<arch_node>(this_node));

	//Add depencendies to graph
	graph.add_dep(tn,left);
	graph.add_dep(tn,right);

	//Add to graph nodes
	this_node->set_left(left);
	this_node->set_right(right);

	//Add this node to result
	result.put(node,tn);
}

void arch_gen::visit_rtl(std::shared_ptr<ast_rtl> node){
	//Propagate dominant datatype
	dominant_dt.put(node,*dominant_dt.get(node->get_parent().lock()));

	//Visit children
	visit(node->get_children());

	//Get pid
	unsigned int orig_var_id(*var_id.get(node));
	std::shared_ptr<var_property> var_prop(vcscope->get()[orig_var_id]);
	unsigned int pid(var_prop->pid);

	//Get left and right
	std::shared_ptr<arch_node> left(*result.get(node->get_left()));
	std::shared_ptr<arch_node> right(*result.get(node->get_right()));

	//Error checking, right must be arch_num
	if(right->get_op()!=arch_node::NUM){ //0->const
		error_handler::rtl_right_not_const(var_prop->name);
	}
	
	//Error checking, right must be integer (not frac)
	if(right->get_datatype().get_frac()!=0){
		error_handler::rtl_frac_rotate(var_prop->name);
	}
	
	//Error checking, right must be unsigned
	if(right->get_datatype().get_datatype()!=datatype::UNSIGNED){
		error_handler::rtl_right_not_unsigned(var_prop->name);
	}
	
	//Error checking, left must be unsigned
	if(left->get_datatype().get_datatype()!=datatype::UNSIGNED){
		error_handler::rtl_left_not_unsigned(var_prop->name);
	}

	//Datatype is the same as left operand
	datatype dt(left->get_datatype());

	//Create new node
	std::shared_ptr<arch_rtl> this_node(std::make_shared<arch_rtl>(var_cnt++,pid,dt));
	std::shared_ptr<arch_node> tn(std::static_pointer_cast<arch_node>(this_node));

	//Add depencendies to graph
	graph.add_dep(tn,left);
	graph.add_dep(tn,right);

	//Add to graph nodes
	this_node->set_left(left);
	this_node->set_right(right);

	//Add this node to result
	result.put(node,tn);
}

void arch_gen::visit_rtr(std::shared_ptr<ast_rtr> node){
	//Propagate dominant datatype
	dominant_dt.put(node,*dominant_dt.get(node->get_parent().lock()));

	//Visit children
	visit(node->get_children());

	//Get pid
	unsigned int orig_var_id(*var_id.get(node));
	std::shared_ptr<var_property> var_prop(vcscope->get()[orig_var_id]);
	unsigned int pid(var_prop->pid);

	//Get left and right
	std::shared_ptr<arch_node> left(*result.get(node->get_left()));
	std::shared_ptr<arch_node> right(*result.get(node->get_right()));

	//Error checking, right must be arch_num
	if(right->get_op()!=arch_node::NUM){ //0->const
		error_handler::rtr_right_not_const(var_prop->name);
	}
	
	//Error checking, right must be integer (not frac)
	if(right->get_datatype().get_frac()!=0){
		error_handler::rtr_frac_rotate(var_prop->name);
	}
	
	//Error checking, right must be unsigned
	if(right->get_datatype().get_datatype()!=datatype::UNSIGNED){
		error_handler::rtr_right_not_unsigned(var_prop->name);
	}
	
	//Error checking, left must be unsigned
	if(left->get_datatype().get_datatype()!=datatype::UNSIGNED){
		error_handler::rtr_left_not_unsigned(var_prop->name);
	}

	//Datatype is the same as left operand
	datatype dt(left->get_datatype());

	//Create new node
	std::shared_ptr<arch_rtr> this_node(std::make_shared<arch_rtr>(var_cnt++,pid,dt));
	std::shared_ptr<arch_node> tn(std::static_pointer_cast<arch_node>(this_node));

	//Add depencendies to graph
	graph.add_dep(tn,left);
	graph.add_dep(tn,right);

	//Add to graph nodes
	this_node->set_left(left);
	this_node->set_right(right);

	//Add this node to result
	result.put(node,tn);
}

void arch_gen::visit_and(std::shared_ptr<ast_and> node){
	//Propagate dominant datatype
	dominant_dt.put(node,*dominant_dt.get(node->get_parent().lock()));

	//Visit children
	visit(node->get_children());

	//Get pid
	unsigned int orig_var_id(*var_id.get(node));
	std::shared_ptr<var_property> var_prop(vcscope->get()[orig_var_id]);
	unsigned int pid(var_prop->pid);

	//Get left and right
	std::shared_ptr<arch_node> left(*result.get(node->get_left()));
	std::shared_ptr<arch_node> right(*result.get(node->get_right()));

	datatype dtl(left->get_datatype());
	datatype dtr(right->get_datatype());

	//Calculate final datatype
	datatype::dt_types ddt;
	if(dtl.get_datatype()==datatype::SIGNED || dtr.get_datatype()==datatype::SIGNED){
		ddt=datatype::SIGNED;
	}
	else{
		ddt=datatype::UNSIGNED;
	}
	datatype dt(ddt,std::max(dtl.get_integ(),dtr.get_integ()),std::max(dtl.get_frac(),dtr.get_frac()));

	//Create new node
	std::shared_ptr<arch_and> this_node(std::make_shared<arch_and>(var_cnt++,pid,dt));
	std::shared_ptr<arch_node> tn(std::static_pointer_cast<arch_node>(this_node));

	//Add depencendies to graph
	graph.add_dep(tn,left);
	graph.add_dep(tn,right);

	//Add to graph nodes
	this_node->set_left(left);
	this_node->set_right(right);

	//Add this node to result
	result.put(node,tn);
}

void arch_gen::visit_or(std::shared_ptr<ast_or> node){
	//Propagate dominant datatype
	dominant_dt.put(node,*dominant_dt.get(node->get_parent().lock()));

	//Visit children
	visit(node->get_children());

	//Get pid
	unsigned int orig_var_id(*var_id.get(node));
	std::shared_ptr<var_property> var_prop(vcscope->get()[orig_var_id]);
	unsigned int pid(var_prop->pid);

	//Get left and right
	std::shared_ptr<arch_node> left(*result.get(node->get_left()));
	std::shared_ptr<arch_node> right(*result.get(node->get_right()));

	datatype dtl(left->get_datatype());
	datatype dtr(right->get_datatype());

	//Calculate final datatype
	datatype::dt_types ddt;
	if(dtl.get_datatype()==datatype::SIGNED || dtr.get_datatype()==datatype::SIGNED){
		ddt=datatype::SIGNED;
	}
	else{
		ddt=datatype::UNSIGNED;
	}
	datatype dt(ddt,std::max(dtl.get_integ(),dtr.get_integ()),std::max(dtl.get_frac(),dtr.get_frac()));

	//Create new node
	std::shared_ptr<arch_or> this_node(std::make_shared<arch_or>(var_cnt++,pid,dt));
	std::shared_ptr<arch_node> tn(std::static_pointer_cast<arch_node>(this_node));

	//Add depencendies to graph
	graph.add_dep(tn,left);
	graph.add_dep(tn,right);

	//Add to graph nodes
	this_node->set_left(left);
	this_node->set_right(right);

	//Add this node to result
	result.put(node,tn);
}

void arch_gen::visit_xor(std::shared_ptr<ast_xor> node){
	//Propagate dominant datatype
	dominant_dt.put(node,*dominant_dt.get(node->get_parent().lock()));

	//Visit children
	visit(node->get_children());

	//Get pid
	unsigned int orig_var_id(*var_id.get(node));
	std::shared_ptr<var_property> var_prop(vcscope->get()[orig_var_id]);
	unsigned int pid(var_prop->pid);

	//Get left and right
	std::shared_ptr<arch_node> left(*result.get(node->get_left()));
	std::shared_ptr<arch_node> right(*result.get(node->get_right()));

	datatype dtl(left->get_datatype());
	datatype dtr(right->get_datatype());

	//Calculate final datatype
	datatype::dt_types ddt;
	if(dtl.get_datatype()==datatype::SIGNED || dtr.get_datatype()==datatype::SIGNED){
		ddt=datatype::SIGNED;
	}
	else{
		ddt=datatype::UNSIGNED;
	}
	datatype dt(ddt,std::max(dtl.get_integ(),dtr.get_integ()),std::max(dtl.get_frac(),dtr.get_frac()));

	//Create new node
	std::shared_ptr<arch_xor> this_node(std::make_shared<arch_xor>(var_cnt++,pid,dt));
	std::shared_ptr<arch_node> tn(std::static_pointer_cast<arch_node>(this_node));

	//Add depencendies to graph
	graph.add_dep(tn,left);
	graph.add_dep(tn,right);

	//Add to graph nodes
	this_node->set_left(left);
	this_node->set_right(right);

	//Add this node to result
	result.put(node,tn);
}

void arch_gen::visit_not(std::shared_ptr<ast_not> node){
	//Propagate dominant datatype
	dominant_dt.put(node,*dominant_dt.get(node->get_parent().lock()));

	//Visit children
	visit(node->get_children());

	//Get pid
	unsigned int orig_var_id(*var_id.get(node));
	std::shared_ptr<var_property> var_prop(vcscope->get()[orig_var_id]);
	unsigned int pid(var_prop->pid);

	//Get target
	std::shared_ptr<arch_node> target(*result.get(node->get_target()));

	//The final datatype is the same as the target
	datatype dt(target->get_datatype());

	//Create new node
	std::shared_ptr<arch_not> this_node(std::make_shared<arch_not>(var_cnt++,pid,dt));
	std::shared_ptr<arch_node> tn(std::static_pointer_cast<arch_node>(this_node));

	//Add depencendies to graph
	graph.add_dep(tn,target);

	//Add to graph nodes
	this_node->set_target(target);

	//Add this node to result
	result.put(node,tn);
}

void arch_gen::visit_gap(std::shared_ptr<ast_gap> node){
	//TODO rise error
	visit(node->get_children());
}

void arch_gen::visit_comp(std::shared_ptr<ast_comp> node){
	//Propagate dominant datatype
	dominant_dt.put(node,*dominant_dt.get(node->get_parent().lock()));

	//Visit children
	visit(node->get_children());

	//Get pid
	unsigned int orig_var_id(*var_id.get(node));
	std::shared_ptr<var_property> var_prop(vcscope->get()[orig_var_id]);
	unsigned int pid(var_prop->pid);

	//Set datatype
	datatype dt(datatype::UNSIGNED,1,0);

	//Create new node
	std::shared_ptr<arch_comp> this_node(std::make_shared<arch_comp>(var_cnt++,node->get_logic_op(),pid,dt));
	std::shared_ptr<arch_node> tn(std::static_pointer_cast<arch_node>(this_node));

	//Add depencendies to graph
	std::shared_ptr<arch_node> left(*result.get(node->get_left()));
	std::shared_ptr<arch_node> right(*result.get(node->get_right()));
	graph.add_dep(tn,left);
	graph.add_dep(tn,right);

	//Add to graph nodes
	this_node->set_left(left);
	this_node->set_right(right);

	//Add this node to result
	result.put(node,tn);
}

void arch_gen::visit_prop(std::shared_ptr<ast_prop> node){
	//TODO rise error
	visit(node->get_children());
	/*
	//Visit children
	visit(node->get_children());

	//Get pid and datatype
	unsigned int orig_var_id(*var_id.get(node));
	std::shared_ptr<var_property> var_prop(vcscope->get()[orig_var_id]);
	unsigned int pid(var_prop->pid);
	datatype dt(var_prop->type);

	//Create new node
	std::shared_ptr<arch_prop> this_node(std::make_shared<arch_prop>(var_cnt++,pid,dt));
	std::shared_ptr<arch_node> tn(std::static_pointer_cast<arch_node>(this_node));

	//Add depencendies to graph
	std::shared_ptr<arch_node> target(*result.get(node->get_target()));
	graph.add_dep(tn,target);

	//Add to graph nodes
	this_node->set_target(target);

	//Add this node to result
	result.put(node,tn);
	*/
}

void arch_gen::visit_mod(std::shared_ptr<ast_mod> node){
	//Propagate dominant datatype
	dominant_dt.put(node,*dominant_dt.get(node->get_parent().lock()));

	//Visit children
	visit(node->get_children());

	//Get pid
	unsigned int orig_var_id(*var_id.get(node));
	std::shared_ptr<var_property> var_prop(vcscope->get()[orig_var_id]);
	unsigned int pid(var_prop->pid);

	//Get left and right
	std::shared_ptr<arch_node> left(*result.get(node->get_left()));
	std::shared_ptr<arch_node> right(*result.get(node->get_right()));

	//Error checking, left must be integer (not frac)
	if(left->get_datatype().get_frac()!=0){
		error_handler::mod_frac_left(var_prop->name);
	}

	//Error checking, right must be integer (not frac)
	if(right->get_datatype().get_frac()!=0){
		error_handler::mod_frac_right(var_prop->name);
	}

	datatype dtl(left->get_datatype());
	datatype dtr(right->get_datatype());

	//Calculate final datatype
	datatype::dt_types ddt;
	if(dtl.get_datatype()==datatype::SIGNED || dtr.get_datatype()==datatype::SIGNED){
		ddt=datatype::SIGNED;
	}
	else{
		ddt=datatype::UNSIGNED;
	}
	datatype dt(ddt,std::min(dtl.get_integ(),dtr.get_integ()),0);

	//Create new node
	std::shared_ptr<arch_mod> this_node(std::make_shared<arch_mod>(var_cnt++,pid,dt));
	std::shared_ptr<arch_node> tn(std::static_pointer_cast<arch_node>(this_node));

	//Add depencendies to graph
	graph.add_dep(tn,left);
	graph.add_dep(tn,right);

	//Add to graph nodes
	this_node->set_left(left);
	this_node->set_right(right);

	//Add this node to result
	result.put(node,tn);
}


void arch_gen::visit_bin(std::shared_ptr<ast_bin_arith> node){
	(void) node;
	assert(false); //Should not be visited
}

void arch_gen::visit_unary(std::shared_ptr<ast_unary_arith> node){
	(void) node;
	assert(false); //Should not be visited
}
