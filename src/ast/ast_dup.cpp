#include "ast.h"

std::shared_ptr<ast> ast_id::duplicate(){
	//Construct new node
	std::shared_ptr<ast_id> nai(std::make_shared<ast_id>(*this));
	std::weak_ptr<ast> wnai(nai);

	if(index!=nullptr){
		//Duplicate index
		std::shared_ptr<ast_arith> i(std::static_pointer_cast<ast_arith>(index->duplicate()));

		//Fix index
		i->set_parent(wnai);
		nai->set_index(i);
	}

	if(instant!=nullptr){
		//Duplicate index
		std::shared_ptr<ast_arith> i(std::static_pointer_cast<ast_arith>(instant->duplicate()));

		//Fix index
		i->set_parent(wnai);
		nai->set_instant(i);
	}

	return nai;
}

std::shared_ptr<ast> ast_start::duplicate(){
	//Construct new node
	std::shared_ptr<ast_start> naa(std::make_shared<ast_start>(*this));
	std::weak_ptr<ast> wnaa(naa);
	
	//Duplicate and fix code
	std::vector<std::shared_ptr<ast>> &cc(naa->get_code());
	for(auto it=cc.begin();it<cc.end();it++){
		*it=(*it)->duplicate();	//inplace replace on the copied vector
		(*it)->set_parent(wnaa); //fix parent
	}

	return naa;
}

std::shared_ptr<ast> ast_include::duplicate(){
	//Construct new node and return it
	//return std::shared_ptr<ast_include>(std::make_shared<ast_include>(*this));	
	return std::make_shared<ast_include>(*this);
}

std::shared_ptr<ast> ast_cblk_decl::duplicate(){
	//Construct new node
	std::shared_ptr<ast_cblk_decl> nacd(std::make_shared<ast_cblk_decl>(*this));
	std::weak_ptr<ast> wnacd(nacd);

	//Duplicate and fix const_list
	std::vector<std::shared_ptr<ast_id>> &cl(nacd->get_const_list());
	for(auto it=cl.begin();it<cl.end();it++){
		std::shared_ptr<ast> ai((*it)->duplicate());
		*it=std::static_pointer_cast<ast_id>(ai);	//inplace replace on the copied vector
		(*it)->set_parent(wnacd); //fix parent
	}

	//Duplicate and fix in_list
	std::vector<std::shared_ptr<ast_id>> &il(nacd->get_in_list());
	for(auto it=il.begin();it<il.end();it++){
		std::shared_ptr<ast> ai((*it)->duplicate());
		*it=std::static_pointer_cast<ast_id>(ai);	//inplace replace on the copied vector
		(*it)->set_parent(wnacd); //fix parent
	}

	//Duplicate and fix out_list
	std::vector<std::shared_ptr<ast_id>> &ol(nacd->get_out_list());
	for(auto it=ol.begin();it<ol.end();it++){
		std::shared_ptr<ast> ai((*it)->duplicate());
		*it=std::static_pointer_cast<ast_id>(ai);	//inplace replace on the copied vector
		(*it)->set_parent(wnacd); //fix parent
	}
	
	//Duplicate and fix code
	std::vector<std::shared_ptr<ast>> &cc(nacd->get_code());
	for(auto it=cc.begin();it<cc.end();it++){
		*it=(*it)->duplicate();	//inplace replace on the copied vector
		(*it)->set_parent(wnacd); //fix parent
	}

	return nacd;
}

std::shared_ptr<ast> ast_s_cblk_inst::duplicate(){
	//Construct new node
	std::shared_ptr<ast_s_cblk_inst> nsc(std::make_shared<ast_s_cblk_inst>(*this));
	std::weak_ptr<ast> wnsc(nsc);

	//Duplicate and fix const_list
	std::vector<std::shared_ptr<ast_arith>> &cl(nsc->get_const_list());
	for(auto it=cl.begin();it<cl.end();it++){
		std::shared_ptr<ast> ai((*it)->duplicate());
		*it=std::static_pointer_cast<ast_arith>(ai);	//inplace replace on the copied vector
		(*it)->set_parent(wnsc); //fix parent
	}

	//Duplicate and fix in_list
	std::vector<std::shared_ptr<ast_arith>> &il(nsc->get_in_list());
	for(auto it=il.begin();it<il.end();it++){
		std::shared_ptr<ast> ai((*it)->duplicate());
		*it=std::static_pointer_cast<ast_arith>(ai);	//inplace replace on the copied vector
		(*it)->set_parent(wnsc); //fix parent
	}

	return nsc;
}

std::shared_ptr<ast> ast_cblk_inst::duplicate(){
	//Construct new node
	std::shared_ptr<ast_cblk_inst> nci(std::make_shared<ast_cblk_inst>(*this));
	std::weak_ptr<ast> wnci(nci);

	//Duplicate and fix const_list
	std::vector<std::shared_ptr<ast_arith>> &cl(nci->get_const_list());
	for(auto it=cl.begin();it<cl.end();it++){
		std::shared_ptr<ast> ai((*it)->duplicate());
		*it=std::static_pointer_cast<ast_arith>(ai);	//inplace replace on the copied vector
		(*it)->set_parent(wnci); //fix parent
	}

	//Duplicate and fix in_list
	std::vector<std::shared_ptr<ast_arith>> &il(nci->get_in_list());
	for(auto it=il.begin();it<il.end();it++){
		std::shared_ptr<ast> ai((*it)->duplicate());
		*it=std::static_pointer_cast<ast_arith>(ai);	//inplace replace on the copied vector
		(*it)->set_parent(wnci); //fix parent
	}

	//Duplicate and fix out_list
	std::vector<std::shared_ptr<ast_id>> &ol(nci->get_out_list());
	for(auto it=ol.begin();it<ol.end();it++){
		std::shared_ptr<ast> ai((*it)->duplicate());
		*it=std::static_pointer_cast<ast_id>(ai);	//inplace replace on the copied vector
		(*it)->set_parent(wnci); //fix parent
	}

	return nci;
}

std::shared_ptr<ast> ast_pulse::duplicate(){
	//Construct new node and return it
	return std::shared_ptr<ast_pulse>(std::make_shared<ast_pulse>(*this));	
}

std::shared_ptr<ast> ast_if::duplicate(){
	//Construct new node
	std::shared_ptr<ast_if> ni(std::make_shared<ast_if>(*this));
	std::weak_ptr<ast> wni(ni);

	//Duplicate and fix if_logic
	std::shared_ptr<ast_arith> il(std::static_pointer_cast<ast_arith>(logic_expr->duplicate()));
	il->set_parent(wni); //fix parent
	ni->set_if_logic(il);

	//Duplicate and fix iftrue code
	std::vector<std::shared_ptr<ast>> &cl(ni->get_true_code());
	for(auto it=cl.begin();it<cl.end();it++){
		*it=(*it)->duplicate();	//inplace replace on the copied vector
		(*it)->set_parent(wni); //fix parent
	}

	//Duplicate and fix elseif
	std::vector<std::pair<std::shared_ptr<ast_arith>,std::vector<std::shared_ptr<ast>>>> &ei(ni->get_elseif());
	for(auto it0=ei.begin();it0<ei.end();it0++){

		//Duplicate each elseif condition
		std::pair<std::shared_ptr<ast_arith>,std::vector<std::shared_ptr<ast>>> &e(*it0);
		e.first=std::static_pointer_cast<ast_arith>(e.first->duplicate());
		e.first->set_parent(wni); //fix parent

		//Duplicate each for each elsif the code
		std::vector<std::shared_ptr<ast>> &eic(e.second);
		for(auto it1=eic.begin();it1<eic.end();it1++){
			*it1=(*it1)->duplicate(); //inplace replace on the copied vector	
			(*it1)->set_parent(wni); //fix parent
		}
	}

	//Duplicate and fix else code
	std::vector<std::shared_ptr<ast>> &ec(ni->get_else_code());
	for(auto it=ec.begin();it<ec.end();it++){
		*it=(*it)->duplicate();	//inplace replace on the copied vector
		(*it)->set_parent(wni); //fix parent
	}
	return ni;
}

std::shared_ptr<ast> ast_for::duplicate(){
	//Construct new node
	std::shared_ptr<ast_for> nf(std::make_shared<ast_for>(*this));
	std::weak_ptr<ast> wnf(nf);

	//Duplicate and fix lower bound 
	std::shared_ptr<ast_arith> lb(std::static_pointer_cast<ast_arith>(lower_bound->duplicate()));
	lb->set_parent(wnf); //fix parent
	nf->set_lower(lb);

	//Duplicate and fix upper bound 
	std::shared_ptr<ast_arith> ub(std::static_pointer_cast<ast_arith>(upper_bound->duplicate()));
	ub->set_parent(wnf); //fix parent
	nf->set_upper(ub);

	if(increment!=nullptr){
		//Duplicate and fix increment 
		std::shared_ptr<ast_arith> inc(std::static_pointer_cast<ast_arith>(increment->duplicate()));
		inc->set_parent(wnf); //fix parent
		nf->set_increment(inc);
	}

	//Duplicate and fix for code
	std::vector<std::shared_ptr<ast>> &cb(nf->get_code_block());
	for(auto it=cb.begin();it<cb.end();it++){
		*it=(*it)->duplicate();	//inplace replace on the copied vector
		(*it)->set_parent(wnf); //fix parent
	}

	return nf;
}

std::shared_ptr<ast> ast_decl::duplicate(){
	//Construct new node
	std::shared_ptr<ast_decl> dl(std::make_shared<ast_decl>(*this));
	std::weak_ptr<ast> wdl(dl);

	//Duplicate and fix var 
	std::shared_ptr<ast_id> vr(std::static_pointer_cast<ast_id>(var->duplicate()));
	vr->set_parent(wdl); //fix parent
	dl->set_var(vr);

	if(attrib!=nullptr){
		//Duplicate and fix attrib 
		std::shared_ptr<ast_arith> at(std::static_pointer_cast<ast_arith>(attrib->duplicate()));
		at->set_parent(wdl); //fix parent
		dl->set_attrib(at);
	}

	return dl;	
}

std::shared_ptr<ast> ast_assign::duplicate(){
	//Construct new node
	std::shared_ptr<ast_assign> ass(std::make_shared<ast_assign>(*this));
	std::weak_ptr<ast> wass(ass);

	//Duplicate and fix var 
	std::shared_ptr<ast_id> vr(std::static_pointer_cast<ast_id>(var->duplicate()));
	vr->set_parent(wass); //fix parent
	ass->set_var(vr);

	//Duplicate and fix attrib 
	std::shared_ptr<ast_arith> at(std::static_pointer_cast<ast_arith>(attrib->duplicate()));
	at->set_parent(wass); //fix parent
	ass->set_attrib(at);

	return ass;	
}

std::shared_ptr<ast> ast_unary_arith::duplicate_unary(std::shared_ptr<ast_unary_arith>& dup_self){
	std::weak_ptr<ast> wdup(dup_self);

	//Duplicate and fix target 
	std::shared_ptr<ast_arith> ta(std::static_pointer_cast<ast_arith>(target->duplicate()));
	ta->set_parent(wdup); //fix parent
	dup_self->set_target(ta);

	return dup_self;	
}

std::shared_ptr<ast> ast_bin_arith::duplicate_binary(std::shared_ptr<ast_bin_arith>& dup_self){
	std::weak_ptr<ast> wdup(dup_self);

	//Duplicate and fix left 
	std::shared_ptr<ast_arith> le(std::static_pointer_cast<ast_arith>(left->duplicate()));
	le->set_parent(wdup); //fix parent
	dup_self->set_left(le);

	//Duplicate and fix righ 
	std::shared_ptr<ast_arith> ri(std::static_pointer_cast<ast_arith>(right->duplicate()));
	ri->set_parent(wdup); //fix parent
	dup_self->set_right(ri);

	return dup_self;	
}

std::shared_ptr<ast> ast_comp::duplicate(){
	//Construct new node
	std::shared_ptr<ast_bin_arith> dup_self(std::make_shared<ast_comp>(*this));
	return duplicate_binary(dup_self);
}

std::shared_ptr<ast> ast_cast::duplicate(){
	//Construct new node
	std::shared_ptr<ast_cast> ac(std::make_shared<ast_cast>(*this));
	std::weak_ptr<ast> wac(ac);

	//Duplicate and fix target 
	std::shared_ptr<ast_arith> tar(std::static_pointer_cast<ast_arith>(target->duplicate()));
	tar->set_parent(wac); //fix parent
	ac->set_target(tar);

	return ac;	
}

std::shared_ptr<ast> ast_arrinit::duplicate(){
	//Construct new node
	std::shared_ptr<ast_arrinit> aai(std::make_shared<ast_arrinit>(*this));
	std::weak_ptr<ast> waai(aai);

	//Duplicate and fix vals
	std::vector<std::shared_ptr<ast_arith>> &vs(aai->get_vals());
	for(auto it=vs.begin();it<vs.end();it++){
		std::shared_ptr<ast> v((*it)->duplicate());
		*it=std::static_pointer_cast<ast_arith>(v);	//inplace replace on the copied vector
		(*it)->set_parent(waai); //fix parent
	}

	return aai;
}

std::shared_ptr<ast> ast_num::duplicate(){
	//Construct new node and return it
	return std::make_shared<ast_num>(*this);
}

std::shared_ptr<ast> ast_add::duplicate(){
	std::shared_ptr<ast_bin_arith> dup_self(std::make_shared<ast_add>(*this));
	return duplicate_binary(dup_self);
}

std::shared_ptr<ast> ast_sub::duplicate(){
	std::shared_ptr<ast_bin_arith> dup_self(std::make_shared<ast_sub>(*this));
	return duplicate_binary(dup_self);
}

std::shared_ptr<ast> ast_sim::duplicate(){
	std::shared_ptr<ast_unary_arith> dup_self(std::make_shared<ast_sim>(*this));
	return duplicate_unary(dup_self);
}

std::shared_ptr<ast> ast_mult::duplicate(){
	std::shared_ptr<ast_bin_arith> dup_self(std::make_shared<ast_mult>(*this));
	return duplicate_binary(dup_self);
}

std::shared_ptr<ast> ast_div::duplicate(){
	std::shared_ptr<ast_bin_arith> dup_self(std::make_shared<ast_div>(*this));
	return duplicate_binary(dup_self);
}

std::shared_ptr<ast> ast_shl::duplicate(){
	std::shared_ptr<ast_bin_arith> dup_self(std::make_shared<ast_shl>(*this));
	return duplicate_binary(dup_self);
}

std::shared_ptr<ast> ast_shr::duplicate(){
	std::shared_ptr<ast_bin_arith> dup_self(std::make_shared<ast_shr>(*this));
	return duplicate_binary(dup_self);
}

std::shared_ptr<ast> ast_rtl::duplicate(){
	std::shared_ptr<ast_bin_arith> dup_self(std::make_shared<ast_rtl>(*this));
	return duplicate_binary(dup_self);
}

std::shared_ptr<ast> ast_rtr::duplicate(){
	std::shared_ptr<ast_bin_arith> dup_self(std::make_shared<ast_rtr>(*this));
	return duplicate_binary(dup_self);
}

std::shared_ptr<ast> ast_and::duplicate(){
	std::shared_ptr<ast_bin_arith> dup_self(std::make_shared<ast_and>(*this));
	return duplicate_binary(dup_self);
}

std::shared_ptr<ast> ast_or::duplicate(){
	std::shared_ptr<ast_bin_arith> dup_self(std::make_shared<ast_or>(*this));
	return duplicate_binary(dup_self);
}

std::shared_ptr<ast> ast_xor::duplicate(){
	std::shared_ptr<ast_bin_arith> dup_self(std::make_shared<ast_xor>(*this));
	return duplicate_binary(dup_self);
}

std::shared_ptr<ast> ast_not::duplicate(){
	std::shared_ptr<ast_unary_arith> dup_self(std::make_shared<ast_not>(*this));
	return duplicate_unary(dup_self);
}

std::shared_ptr<ast> ast_gap::duplicate(){
	std::shared_ptr<ast_bin_arith> dup_self(std::make_shared<ast_gap>(*this));
	return duplicate_binary(dup_self);
}

std::shared_ptr<ast> ast_prop::duplicate(){
	std::shared_ptr<ast_unary_arith> dup_self(std::make_shared<ast_prop>(*this));
	return duplicate_unary(dup_self);
}

std::shared_ptr<ast> ast_mod::duplicate(){
	std::shared_ptr<ast_bin_arith> dup_self(std::make_shared<ast_mod>(*this));
	return duplicate_binary(dup_self);
}
