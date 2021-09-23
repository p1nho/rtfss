#include "parse_tree_translator.h"
#include "tree/ParseTree.h"

antlrcpp::Any parse_tree_translator::visitEntry_point(rtfssParser::Entry_pointContext *ctx){
	ptp.put(ctx,start);

	//Traverse all uses
	//TODO do something, like copy tree?
	std::vector<rtfssParser::Use_statContext*> use(ctx->use_stat());
	for(auto it=use.begin();it<use.end();it++){
		std::shared_ptr<ast> c(visit(*it).as<std::shared_ptr<ast>>());
		start->add_code(c);
	}	

	//Traverse all cblocks 
	std::vector<rtfssParser::Cblock_declContext*> cblock=ctx->cblock_decl();
	for(auto it=cblock.begin();it<cblock.end();it++){
		std::shared_ptr<ast> c(visit(*it).as<std::shared_ptr<ast>>());
		start->add_code(c);
	}	

	//Dont traverse EOF

	//No children left to visit
	return std::shared_ptr<ast>(start);
}

antlrcpp::Any parse_tree_translator::visitUse_stat(rtfssParser::Use_statContext *ctx){
	std::shared_ptr<ast> parent(ptp.get(ctx->parent));
	std::weak_ptr<ast> wparent(parent);

	std::string n(ctx->std_id->getText());
	std::shared_ptr<ast_include> ad(std::make_shared<ast_include>(wparent,n));
	ptp.put(ctx,ad);

	//No children left to visit
	return std::shared_ptr<ast>(ad);
}

antlrcpp::Any parse_tree_translator::visitCblock_decl(rtfssParser::Cblock_declContext *ctx){
	std::shared_ptr<ast> parent(ptp.get(ctx->parent));
	std::weak_ptr<ast> wparent(parent);

	std::string cblk_name(ctx->IDENTIFIER()->getText());
	std::string pulse(ctx->pulse_freq()->getText());
	pulse_id pid(pulse);
	std::shared_ptr<ast_cblk_decl> acd(std::make_shared<ast_cblk_decl>(wparent,cblk_name,pid)); 
	ptp.put(ctx,acd);

	//Visit cblock_args, children fill the parent node
	visit(ctx->cblock_args());

	//Visit code_block, and yank to code vector
	acd->set_code(visit(ctx->code_block()).as<std::vector<std::shared_ptr<ast>>>());	

	//No children left to visit
	return std::shared_ptr<ast>(acd);
}

antlrcpp::Any parse_tree_translator::visitCblock_args(rtfssParser::Cblock_argsContext *ctx){
	//This function bridges Cblock_decl and Cblock_decl_args

	std::shared_ptr<ast> parent(ptp.get(ctx->parent));
	std::shared_ptr<ast_cblk_decl> cparent(std::static_pointer_cast<ast_cblk_decl>(parent));
	ptp.put(ctx,ptp.get(ctx->parent)); //Invisible node

	antlr4::tree::ParseTree *consts = ctx->const_args;
	antlr4::tree::ParseTree *ins = ctx->in_args;
	antlr4::tree::ParseTree *outs = ctx->out_args;

	//Get const
	if(consts!=nullptr){
		std::vector<std::shared_ptr<ast>> tmp(visit(ctx->const_args).as<std::vector<std::shared_ptr<ast>>>());
		for(auto it=tmp.begin();it<tmp.end();it++){
			std::shared_ptr<ast_id> v(std::static_pointer_cast<ast_id>(*it));
			cparent->add_const(v);
		}
	}

	//Get ins
	if(ins!=nullptr){
		std::vector<std::shared_ptr<ast>> tmp(visit(ctx->in_args).as<std::vector<std::shared_ptr<ast>>>());
		for(auto it=tmp.begin();it<tmp.end();it++){
			std::shared_ptr<ast_id> v(std::static_pointer_cast<ast_id>(*it));
			cparent->add_in(v);
		}
	}

	//Get const
	if(outs!=nullptr){
		std::vector<std::shared_ptr<ast>> tmp(visit(ctx->out_args).as<std::vector<std::shared_ptr<ast>>>());
		for(auto it=tmp.begin();it<tmp.end();it++){
			std::shared_ptr<ast_id> v(std::static_pointer_cast<ast_id>(*it));
			cparent->add_out(v);
		}
	}

	//Children already visited
	//No valid return
	return nullptr;
}

antlrcpp::Any parse_tree_translator::visitCblock_arg_list(rtfssParser::Cblock_arg_listContext *ctx){

	std::shared_ptr<ast> parent(ptp.get(ctx->parent));
	std::weak_ptr<ast> wparent(parent);
	ptp.put(ctx,ptp.get(ctx->parent)); //Invisible node

	std::vector<rtfssParser::Data_typeContext*> dt(ctx->data_type());
	std::vector<rtfssParser::Var_nameContext*> vn(ctx->var_name());

	std::vector<std::shared_ptr<ast>> arg_ret_list;

	auto idt=dt.begin();
	auto ivn=vn.begin();
	for(;idt<dt.end()&&ivn<vn.end();idt++,ivn++){
		std::string type((*idt)->getText());

		//Visit ast_id
		std::shared_ptr<ast> vn(visit(*ivn).as<std::shared_ptr<ast>>());
		std::shared_ptr<ast_id> vvn(std::static_pointer_cast<ast_id>(vn));
		vvn->set_datatype(type);	

		arg_ret_list.push_back(vn);
	}

	//No children to visit
	return arg_ret_list;
}

antlrcpp::Any parse_tree_translator::visitCode_block(rtfssParser::Code_blockContext *ctx){
	//Returns an vector of shared_ptr<ast> to glue to the parent
	std::shared_ptr<ast> parent(ptp.get(ctx->parent));
	ptp.put(ctx,ptp.get(ctx->parent)); //Bridge parent (this is an invisible node)

	std::vector<std::shared_ptr<ast>> code_block;

	std::vector<rtfssParser::StatmContext*> sc(ctx->statm());
	for(auto it=sc.begin();it<sc.end();it++){
		code_block.push_back(visit(*it).as<std::shared_ptr<ast>>());
	}

	return code_block;
}

antlrcpp::Any parse_tree_translator::visitStatm(rtfssParser::StatmContext *ctx){
	//Returns a shared_ptr<ast> to glue to the parent
	std::shared_ptr<ast> parent(ptp.get(ctx->parent));
	ptp.put(ctx,ptp.get(ctx->parent)); //Bridge parent (this is an invisible node)
	
	return visit(ctx->children[0]).as<std::shared_ptr<ast>>();
}

antlrcpp::Any parse_tree_translator::visitTerminated_statm(rtfssParser::Terminated_statmContext *ctx){
	//Returns a shared_ptr<ast> to glue to the parent
	std::shared_ptr<ast> parent(ptp.get(ctx->parent));
	ptp.put(ctx,ptp.get(ctx->parent)); //Bridge parent (this is an invisible node)

	//std::cout<<ctx->children[0]->getText()<<std::endl;

	return visit(ctx->children[0]).as<std::shared_ptr<ast>>();
}

antlrcpp::Any parse_tree_translator::visitNonterminated_statm(rtfssParser::Nonterminated_statmContext *ctx){
	//Returns a shared_ptr<ast> to glue to the parent
	std::shared_ptr<ast> parent(ptp.get(ctx->parent));
	ptp.put(ctx,ptp.get(ctx->parent)); //Bridge parent (this is an invisible node)

	return visit(ctx->children[0]).as<std::shared_ptr<ast>>();
}

antlrcpp::Any parse_tree_translator::visitIf_statm(rtfssParser::If_statmContext *ctx){
	std::shared_ptr<ast> parent(ptp.get(ctx->parent));
	std::weak_ptr<ast> wparent(parent);

	std::shared_ptr<ast_if> ai(std::make_shared<ast_if>(wparent));
	ptp.put(ctx,ai);

	std::vector<rtfssParser::ExprContext*> ex(ctx->expr());
	std::vector<rtfssParser::Code_blockContext*> cb(ctx->code_block());

	//Visit "if" logic expr
	std::shared_ptr<ast> if_logic(visit(ex[0]).as<std::shared_ptr<ast>>());
	std::shared_ptr<ast_arith> if_logic_arith(std::static_pointer_cast<ast_arith>(if_logic));
	ai->set_if_logic(if_logic_arith);

	//Visit "if" code block
	std::vector<std::shared_ptr<ast>> if_code_block(visit(cb[0]).as<std::vector<std::shared_ptr<ast>>>());
	ai->set_if_code(if_code_block);

	//Visit all "else if"
	auto iex=++ex.begin();
	auto icb=++cb.begin();
	for(;iex<ex.end()&&icb<cb.end();iex++,icb++){
		std::shared_ptr<ast> elseif_logic(visit(*iex).as<std::shared_ptr<ast>>());	
		std::shared_ptr<ast_arith> elseif_logic_arith(std::static_pointer_cast<ast_arith>(elseif_logic));

		std::vector<std::shared_ptr<ast>> elseif_code(visit(*icb).as<std::vector<std::shared_ptr<ast>>>());		

		std::pair<std::shared_ptr<ast_arith>,std::vector<std::shared_ptr<ast>>> p(std::make_pair(elseif_logic_arith,elseif_code));
		ai->add_elseif(p);
	}

	if(icb!=cb.end()){ //"else" statement present (a codeblock without an expression)
		
		std::vector<std::shared_ptr<ast>> else_code_block(visit(*icb).as<std::vector<std::shared_ptr<ast>>>());
		ai->set_else_code(else_code_block);

	}

	//No children left to visit
	return std::shared_ptr<ast>(ai);
}

antlrcpp::Any parse_tree_translator::visitFor_statm(rtfssParser::For_statmContext *ctx){
	std::shared_ptr<ast> parent(ptp.get(ctx->parent));
	std::weak_ptr<ast> wparent(parent);

	std::string for_var(ctx->IDENTIFIER()->getText());
	std::shared_ptr<ast_for> af(std::make_shared<ast_for>(wparent,for_var));
	ptp.put(ctx,af);

	//Visit lower
	antlr4::tree::ParseTree *lower(ctx->expr(0));
	std::shared_ptr<ast> lower_expr(visit(lower).as<std::shared_ptr<ast>>());
	std::shared_ptr<ast_arith> lower_expr_arith(std::static_pointer_cast<ast_arith>(lower_expr));
	af->set_lower(lower_expr_arith);	

	//Visit upper
	antlr4::tree::ParseTree *upper(ctx->expr(1));
	std::shared_ptr<ast> upper_expr(visit(upper).as<std::shared_ptr<ast>>());
	std::shared_ptr<ast_arith> upper_expr_arith(std::static_pointer_cast<ast_arith>(upper_expr));
	af->set_upper(upper_expr_arith);	

	//Visit increment
	if(ctx->expr().size()==3){ //Has increment
		antlr4::tree::ParseTree *inc(ctx->expr(2));
		std::shared_ptr<ast> inc_expr(visit(inc).as<std::shared_ptr<ast>>());
		std::shared_ptr<ast_arith> inc_expr_arith(std::static_pointer_cast<ast_arith>(inc_expr));
		af->set_increment(inc_expr_arith);	
	}

	//Visit code block
	antlr4::tree::ParseTree *code(ctx->code_block());
	std::vector<std::shared_ptr<ast>> code_exprs(visit(code).as<std::vector<std::shared_ptr<ast>>>());
	af->set_code(code_exprs);

	//No children left to visit
	return std::shared_ptr<ast>(af);
}

antlrcpp::Any parse_tree_translator::visitCblock_inst(rtfssParser::Cblock_instContext *ctx){
	std::shared_ptr<ast> parent(ptp.get(ctx->parent));
	std::weak_ptr<ast> wparent(parent);

	//Visit Cblock_inst_short
	std::shared_ptr<ast> asci(visit(ctx->cblock_inst_short()).as<std::shared_ptr<ast>>());
	std::shared_ptr<ast_s_cblk_inst> asci_scblk(std::static_pointer_cast<ast_s_cblk_inst>(asci));
	
	//Get in_list,const_list,cblock_name and pid from Cblock_inst_short
	std::vector<std::shared_ptr<ast_arith>> in_list(asci_scblk->get_in_list());
	std::vector<std::shared_ptr<ast_arith>> const_list(asci_scblk->get_const_list());
	pulse_id pid(asci_scblk->get_pulse_id());
	std::string cblock_name(asci_scblk->get_cblock_name());

	std::shared_ptr<ast_cblk_inst> aci(std::make_shared<ast_cblk_inst>(wparent,cblock_name,pid));
	std::weak_ptr<ast> waci(aci);
	ptp.put(ctx,aci);

	//Fix parent connections on const_list
	for(auto it=const_list.begin();it<const_list.end();it++){
		(*it)->set_parent(waci);
	}

	//Fix parent connections on in_list
	for(auto it=in_list.begin();it<in_list.end();it++){
		(*it)->set_parent(waci);
	}

	aci->set_const(const_list);
	aci->set_in(in_list);

	//Visit out args
	std::vector<rtfssParser::Var_nameContext*> oargs(ctx->oargs);
	for(auto it=oargs.begin();it<oargs.end();it++){

		//Visit ast_id
		std::shared_ptr<ast> vn(visit(*it).as<std::shared_ptr<ast>>());
		std::shared_ptr<ast_id> vvn(std::static_pointer_cast<ast_id>(vn));

		aci->add_out(vvn);
	}

	//No children left to visit
	return std::shared_ptr<ast>(aci);
}

antlrcpp::Any parse_tree_translator::visitCblock_inst_short(rtfssParser::Cblock_inst_shortContext *ctx){
	std::shared_ptr<ast> parent(ptp.get(ctx->parent));
	std::weak_ptr<ast> wparent(parent);

	std::string cblk_name(ctx->IDENTIFIER()->getText());
	std::string cblk_pulse(ctx->pulse_freq()->getText());
	pulse_id pid(cblk_pulse);
	std::shared_ptr<ast_s_cblk_inst> asci(std::make_shared<ast_s_cblk_inst>(wparent,cblk_name,cblk_pulse));
	ptp.put(ctx,asci);

	//Visit const args
	std::vector<rtfssParser::ExprContext*> cargs(ctx->cargs);
	for(auto it=cargs.begin();it<cargs.end();it++){
		std::shared_ptr<ast> cargs_expr(visit(*it).as<std::shared_ptr<ast>>());
		std::shared_ptr<ast_arith> cargs_expr_arith(std::static_pointer_cast<ast_arith>(cargs_expr));
		asci->add_const(cargs_expr_arith);
	}

	//Visit in args
	std::vector<rtfssParser::ExprContext*> iargs(ctx->iargs);
	for(auto it=iargs.begin();it<iargs.end();it++){
		std::shared_ptr<ast> iargs_expr(visit(*it).as<std::shared_ptr<ast>>());
		std::shared_ptr<ast_arith> iargs_expr_arith(std::static_pointer_cast<ast_arith>(iargs_expr));
		asci->add_in(iargs_expr_arith);
	}

	//No children left to visit
	return std::shared_ptr<ast>(asci);
}

antlrcpp::Any parse_tree_translator::visitPulse_decl(rtfssParser::Pulse_declContext *ctx){
	std::shared_ptr<ast> parent(ptp.get(ctx->parent));
	std::weak_ptr<ast> wparent(parent);

	std::string pulse_name(ctx->IDENTIFIER()->getText());
	std::string pulse_literal(ctx->PULSE_LITERAL()->getText());

	std::pair<std::string,std::string> lit(ast_pulse::separate_literal(pulse_literal));
	std::shared_ptr<ast_pulse> ap(std::make_shared<ast_pulse>(wparent,pulse_name,lit.first,lit.second));
	ptp.put(ctx,ap);

	//No children left to visit
	return std::shared_ptr<ast>(ap);
}

antlrcpp::Any parse_tree_translator::visitData_decl(rtfssParser::Data_declContext *ctx){
	std::shared_ptr<ast> parent(ptp.get(ctx->parent));
	std::weak_ptr<ast> wparent(parent);

	std::shared_ptr<ast_decl> ad(std::make_shared<ast_decl>(wparent));
	ptp.put(ctx,ad);

	//Get datatype and pulse name
	std::string data_type(ctx->data_type()->getText());	
	std::string pulse_freq(ctx->pulse_freq()->getText());	

	//Visit ast_id and add to ast_decl
	std::shared_ptr<ast> vn(visit(ctx->stream_id()).as<std::shared_ptr<ast>>());
	std::shared_ptr<ast_id> vvn(std::static_pointer_cast<ast_id>(vn));

	vvn->set_datatype(data_type);
	vvn->set_pulse(pulse_freq);

	ad->set_var(vvn);

	//Get attrib, if existent
	antlr4::tree::ParseTree *expr(ctx->expr());
	if(expr!=nullptr){
		std::shared_ptr<ast> decl_expr(visit(expr).as<std::shared_ptr<ast>>());
		std::shared_ptr<ast_arith> decl_expr_arith(std::static_pointer_cast<ast_arith>(decl_expr));
		ad->set_attrib(decl_expr_arith);	
	}

	//No children left to visit
	return std::shared_ptr<ast>(ad);
}

antlrcpp::Any parse_tree_translator::visitVar_assign(rtfssParser::Var_assignContext *ctx){
	std::shared_ptr<ast> parent(ptp.get(ctx->parent));
	std::weak_ptr<ast> wparent(parent);

	//Get operand and generate ast_assign with shared_ptr
	std::string op(ctx->assign_op()->getText());
	std::shared_ptr<ast_assign> aa(std::make_shared<ast_assign>(wparent,op));
	ptp.put(ctx,aa);

	//Visit ast_id and add to ast_decl
	std::shared_ptr<ast> vn(visit(ctx->stream_id()).as<std::shared_ptr<ast>>());
	std::shared_ptr<ast_id> vvn(std::static_pointer_cast<ast_id>(vn));

	aa->set_var(vvn);

	//Get attrib
	antlr4::tree::ParseTree *expr(ctx->expr());
	std::shared_ptr<ast> decl_expr(visit(expr).as<std::shared_ptr<ast>>());
	std::shared_ptr<ast_arith> decl_expr_arith(std::static_pointer_cast<ast_arith>(decl_expr));
	aa->set_attrib(decl_expr_arith);	

	//No children left to visit
	return std::shared_ptr<ast>(aa);
}

antlrcpp::Any parse_tree_translator::visitAssign_op(rtfssParser::Assign_opContext *ctx){
	//This function should never be called
	(void)ctx;
	assert(false);
	return nullptr; 
}

antlrcpp::Any parse_tree_translator::visitModExpr(rtfssParser::ModExprContext *ctx){
	std::shared_ptr<ast> parent(ptp.get(ctx->parent));
	std::weak_ptr<ast> wparent(parent);

	std::shared_ptr<ast_mod> am(std::make_shared<ast_mod>(wparent));
	ptp.put(ctx,am);

	antlr4::tree::ParseTree *left(ctx->expr(0));
	antlr4::tree::ParseTree *right(ctx->expr(1));

	//Visit right and left expr
	std::shared_ptr<ast> left_expr(visit(left).as<std::shared_ptr<ast>>());
	std::shared_ptr<ast_arith> left_expr_arith(std::static_pointer_cast<ast_arith>(left_expr));
	std::shared_ptr<ast> right_expr(visit(right).as<std::shared_ptr<ast>>());
	std::shared_ptr<ast_arith> right_expr_arith(std::static_pointer_cast<ast_arith>(right_expr));

	am->set_left(left_expr_arith);
	am->set_right(right_expr_arith);

	//No children left to visit
	return std::shared_ptr<ast>(am);
}

antlrcpp::Any parse_tree_translator::visitPropExpr(rtfssParser::PropExprContext *ctx){
	std::shared_ptr<ast> parent(ptp.get(ctx->parent));
	std::weak_ptr<ast> wparent(parent);

	std::shared_ptr<ast_prop> ap(std::make_shared<ast_prop>(wparent));
	ptp.put(ctx,ap);

	antlr4::tree::ParseTree *target(ctx->expr());

	//Visit target 
	std::shared_ptr<ast> target_expr(visit(target).as<std::shared_ptr<ast>>());
	std::shared_ptr<ast_arith> target_expr_arith(std::static_pointer_cast<ast_arith>(target_expr));
	ap->set_target(target_expr_arith);

	//No children left to visit
	return std::shared_ptr<ast>(ap);
}

antlrcpp::Any parse_tree_translator::visitCastAExpr(rtfssParser::CastAExprContext *ctx){
	std::shared_ptr<ast> parent(ptp.get(ctx->parent));
	std::weak_ptr<ast> wparent(parent);

	//Get data_type and construct ast_ast
	std::string dt(ctx->data_type()->getText());
	std::shared_ptr<ast_cast> ac(std::make_shared<ast_cast>(wparent,dt));
	ptp.put(ctx,ac);

	//Visit target expr
	antlr4::tree::ParseTree *target(ctx->expr());
	std::shared_ptr<ast> target_expr(visit(target).as<std::shared_ptr<ast>>());
	std::shared_ptr<ast_arith> target_expr_arith(std::static_pointer_cast<ast_arith>(target_expr));
	ac->set_target(target_expr_arith);

	//No children left to visit
	return std::shared_ptr<ast>(ac);
}

antlrcpp::Any parse_tree_translator::visitSubExpr(rtfssParser::SubExprContext *ctx){
	std::shared_ptr<ast> parent(ptp.get(ctx->parent));
	std::weak_ptr<ast> wparent(parent);

	std::shared_ptr<ast_sub> as(std::make_shared<ast_sub>(wparent));
	ptp.put(ctx,as);

	antlr4::tree::ParseTree *left(ctx->expr(0));
	antlr4::tree::ParseTree *right(ctx->expr(1));

	//Visit right and left expr
	std::shared_ptr<ast> left_expr(visit(left).as<std::shared_ptr<ast>>());
	std::shared_ptr<ast_arith> left_expr_arith(std::static_pointer_cast<ast_arith>(left_expr));
	std::shared_ptr<ast> right_expr(visit(right).as<std::shared_ptr<ast>>());
	std::shared_ptr<ast_arith> right_expr_arith(std::static_pointer_cast<ast_arith>(right_expr));

	as->set_left(left_expr_arith);
	as->set_right(right_expr_arith);

	//No children left to visit
	return std::shared_ptr<ast>(as);
}

antlrcpp::Any parse_tree_translator::visitMultExpr(rtfssParser::MultExprContext *ctx){
	std::shared_ptr<ast> parent(ptp.get(ctx->parent));
	std::weak_ptr<ast> wparent(parent);

	std::shared_ptr<ast_mult> am(std::make_shared<ast_mult>(wparent));
	ptp.put(ctx,am);

	antlr4::tree::ParseTree *left(ctx->expr(0));
	antlr4::tree::ParseTree *right(ctx->expr(1));

	//Visit right and left expr
	std::shared_ptr<ast> left_expr(visit(left).as<std::shared_ptr<ast>>());
	std::shared_ptr<ast_arith> left_expr_arith(std::static_pointer_cast<ast_arith>(left_expr));
	std::shared_ptr<ast> right_expr(visit(right).as<std::shared_ptr<ast>>());
	std::shared_ptr<ast_arith> right_expr_arith(std::static_pointer_cast<ast_arith>(right_expr));

	am->set_left(left_expr_arith);
	am->set_right(right_expr_arith);

	//No children left to visit
	return std::shared_ptr<ast>(am);
}

antlrcpp::Any parse_tree_translator::visitNumExpr(rtfssParser::NumExprContext *ctx){
	std::shared_ptr<ast> parent(ptp.get(ctx->parent));
	std::weak_ptr<ast> wparent(parent);

	//Get NUM_LITERAL and construct ast_num
	std::string num(ctx->NUM_LITERAL()->getText());
	std::shared_ptr<ast_num> an(std::make_shared<ast_num>(wparent,num));
	ptp.put(ctx,an);

	//No children left to visit
	return std::shared_ptr<ast>(an);
}

antlrcpp::Any parse_tree_translator::visitShiftExpr(rtfssParser::ShiftExprContext *ctx){
	std::shared_ptr<ast> parent(ptp.get(ctx->parent));
	std::weak_ptr<ast> wparent(parent);

	std::shared_ptr<ast_bin_arith> as;
	if(ctx->SL()!=nullptr){ //shift left
		as=std::make_shared<ast_shl>(wparent);
	}
	else{ //shift right
		as=std::make_shared<ast_shr>(wparent);
	}
	ptp.put(ctx,as);

	antlr4::tree::ParseTree *left(ctx->expr(0));
	antlr4::tree::ParseTree *right(ctx->expr(1));

	//Visit right and left expr
	std::shared_ptr<ast> left_expr(visit(left).as<std::shared_ptr<ast>>());
	std::shared_ptr<ast_arith> left_expr_arith(std::static_pointer_cast<ast_arith>(left_expr));
	std::shared_ptr<ast> right_expr(visit(right).as<std::shared_ptr<ast>>());
	std::shared_ptr<ast_arith> right_expr_arith(std::static_pointer_cast<ast_arith>(right_expr));

	as->set_left(left_expr_arith);
	as->set_right(right_expr_arith);

	//No children left to visit
	return std::shared_ptr<ast>(as);
}

antlrcpp::Any parse_tree_translator::visitRotExpr(rtfssParser::RotExprContext *ctx){
	std::shared_ptr<ast> parent(ptp.get(ctx->parent));
	std::weak_ptr<ast> wparent(parent);

	std::shared_ptr<ast_bin_arith> as;
	if(ctx->RL()!=nullptr){ //rotate left
		as=std::make_shared<ast_rtl>(wparent);
	}
	else{ //rotate right
		as=std::make_shared<ast_rtr>(wparent);
	}
	ptp.put(ctx,as);

	antlr4::tree::ParseTree *left(ctx->expr(0));
	antlr4::tree::ParseTree *right(ctx->expr(1));

	//Visit right and left expr
	std::shared_ptr<ast> left_expr(visit(left).as<std::shared_ptr<ast>>());
	std::shared_ptr<ast_arith> left_expr_arith(std::static_pointer_cast<ast_arith>(left_expr));
	std::shared_ptr<ast> right_expr(visit(right).as<std::shared_ptr<ast>>());
	std::shared_ptr<ast_arith> right_expr_arith(std::static_pointer_cast<ast_arith>(right_expr));

	as->set_left(left_expr_arith);
	as->set_right(right_expr_arith);

	//No children left to visit
	return std::shared_ptr<ast>(as);
}

antlrcpp::Any parse_tree_translator::visitAndExpr(rtfssParser::AndExprContext *ctx){
	std::shared_ptr<ast> parent(ptp.get(ctx->parent));
	std::weak_ptr<ast> wparent(parent);

	std::shared_ptr<ast_and> am(std::make_shared<ast_and>(wparent));
	ptp.put(ctx,am);

	antlr4::tree::ParseTree *left(ctx->expr(0));
	antlr4::tree::ParseTree *right(ctx->expr(1));

	//Visit right and left expr
	std::shared_ptr<ast> left_expr(visit(left).as<std::shared_ptr<ast>>());
	std::shared_ptr<ast_arith> left_expr_arith(std::static_pointer_cast<ast_arith>(left_expr));
	std::shared_ptr<ast> right_expr(visit(right).as<std::shared_ptr<ast>>());
	std::shared_ptr<ast_arith> right_expr_arith(std::static_pointer_cast<ast_arith>(right_expr));

	am->set_left(left_expr_arith);
	am->set_right(right_expr_arith);

	//No children left to visit
	return std::shared_ptr<ast>(am);
}

antlrcpp::Any parse_tree_translator::visitOrExpr(rtfssParser::OrExprContext *ctx){
	std::shared_ptr<ast> parent(ptp.get(ctx->parent));
	std::weak_ptr<ast> wparent(parent);

	std::shared_ptr<ast_or> am(std::make_shared<ast_or>(wparent));
	ptp.put(ctx,am);

	antlr4::tree::ParseTree *left(ctx->expr(0));
	antlr4::tree::ParseTree *right(ctx->expr(1));

	//Visit right and left expr
	std::shared_ptr<ast> left_expr(visit(left).as<std::shared_ptr<ast>>());
	std::shared_ptr<ast_arith> left_expr_arith(std::static_pointer_cast<ast_arith>(left_expr));
	std::shared_ptr<ast> right_expr(visit(right).as<std::shared_ptr<ast>>());
	std::shared_ptr<ast_arith> right_expr_arith(std::static_pointer_cast<ast_arith>(right_expr));

	am->set_left(left_expr_arith);
	am->set_right(right_expr_arith);

	//No children left to visit
	return std::shared_ptr<ast>(am);
}

antlrcpp::Any parse_tree_translator::visitXorExpr(rtfssParser::XorExprContext *ctx){
	std::shared_ptr<ast> parent(ptp.get(ctx->parent));
	std::weak_ptr<ast> wparent(parent);

	std::shared_ptr<ast_xor> am(std::make_shared<ast_xor>(wparent));
	ptp.put(ctx,am);

	antlr4::tree::ParseTree *left(ctx->expr(0));
	antlr4::tree::ParseTree *right(ctx->expr(1));

	//Visit right and left expr
	std::shared_ptr<ast> left_expr(visit(left).as<std::shared_ptr<ast>>());
	std::shared_ptr<ast_arith> left_expr_arith(std::static_pointer_cast<ast_arith>(left_expr));
	std::shared_ptr<ast> right_expr(visit(right).as<std::shared_ptr<ast>>());
	std::shared_ptr<ast_arith> right_expr_arith(std::static_pointer_cast<ast_arith>(right_expr));

	am->set_left(left_expr_arith);
	am->set_right(right_expr_arith);

	//No children left to visit
	return std::shared_ptr<ast>(am);
}

antlrcpp::Any parse_tree_translator::visitNotExpr(rtfssParser::NotExprContext *ctx){
	std::shared_ptr<ast> parent(ptp.get(ctx->parent));
	std::weak_ptr<ast> wparent(parent);

	std::shared_ptr<ast_not> as(std::make_shared<ast_not>(wparent));
	ptp.put(ctx,as);

	antlr4::tree::ParseTree *target(ctx->expr());

	//Visit target 
	std::shared_ptr<ast> target_expr(visit(target).as<std::shared_ptr<ast>>());
	std::shared_ptr<ast_arith> target_expr_arith(std::static_pointer_cast<ast_arith>(target_expr));
	as->set_target(target_expr_arith);

	//No children left to visit
	return std::shared_ptr<ast>(as);
}

antlrcpp::Any parse_tree_translator::visitParExpr(rtfssParser::ParExprContext *ctx){
	//Returns a shared_ptr<ast> to glue to the parent
	std::shared_ptr<ast> parent(ptp.get(ctx->parent));
	ptp.put(ctx,ptp.get(ctx->parent)); //Bridge parent (this is an invisible node)
	
	return visit(ctx->expr()).as<std::shared_ptr<ast>>();
}

antlrcpp::Any parse_tree_translator::visitSidExpr(rtfssParser::SidExprContext *ctx){
	std::shared_ptr<ast> parent(ptp.get(ctx->parent));
	std::weak_ptr<ast> wparent(parent);
	ptp.put(ctx,ptp.get(ctx->parent)); //Bridge parent (this is an invisible node)

	//Visit ast_id
	std::shared_ptr<ast> vn(visit(ctx->stream_id()).as<std::shared_ptr<ast>>());
	std::shared_ptr<ast_id> vvn(std::static_pointer_cast<ast_id>(vn));

	//No children left to visit
	return std::shared_ptr<ast>(vvn);
}

antlrcpp::Any parse_tree_translator::visitCastBExpr(rtfssParser::CastBExprContext *ctx){
	std::shared_ptr<ast> parent(ptp.get(ctx->parent));
	std::weak_ptr<ast> wparent(parent);

	//Get data_type and construct ast_ast
	std::string dt(ctx->data_type()->getText());
	std::shared_ptr<ast_cast> ac(std::make_shared<ast_cast>(wparent,dt));
	ptp.put(ctx,ac);

	//Visit target expr
	antlr4::tree::ParseTree *target(ctx->expr());
	std::shared_ptr<ast> target_expr(visit(target).as<std::shared_ptr<ast>>());
	std::shared_ptr<ast_arith> target_expr_arith(std::static_pointer_cast<ast_arith>(target_expr));
	ac->set_target(target_expr_arith);

	//No children left to visit
	return std::shared_ptr<ast>(ac);
}

antlrcpp::Any parse_tree_translator::visitCblkExpr(rtfssParser::CblkExprContext *ctx){
	//Returns a shared_ptr<ast> to glue to the parent
	std::shared_ptr<ast> parent(ptp.get(ctx->parent));
	ptp.put(ctx,ptp.get(ctx->parent)); //Bridge parent (this is an invisible node)
	
	return visit(ctx->cblock_inst_short()).as<std::shared_ptr<ast>>();
}

antlrcpp::Any parse_tree_translator::visitAddExpr(rtfssParser::AddExprContext *ctx){
	std::shared_ptr<ast> parent(ptp.get(ctx->parent));
	std::weak_ptr<ast> wparent(parent);

	std::shared_ptr<ast_add> aa(std::make_shared<ast_add>(wparent));
	ptp.put(ctx,aa);

	antlr4::tree::ParseTree *left(ctx->expr(0));
	antlr4::tree::ParseTree *right(ctx->expr(1));

	//Visit right and left expr
	std::shared_ptr<ast> left_expr(visit(left).as<std::shared_ptr<ast>>());
	std::shared_ptr<ast_arith> left_expr_arith(std::static_pointer_cast<ast_arith>(left_expr));
	std::shared_ptr<ast> right_expr(visit(right).as<std::shared_ptr<ast>>());
	std::shared_ptr<ast_arith> right_expr_arith(std::static_pointer_cast<ast_arith>(right_expr));

	aa->set_left(left_expr_arith);
	aa->set_right(right_expr_arith);

	//No children left to visit
	return std::shared_ptr<ast>(aa);
}

antlrcpp::Any parse_tree_translator::visitNegExpr(rtfssParser::NegExprContext *ctx){
	std::shared_ptr<ast> parent(ptp.get(ctx->parent));
	std::weak_ptr<ast> wparent(parent);

	std::shared_ptr<ast_sim> as(std::make_shared<ast_sim>(wparent));
	ptp.put(ctx,as);

	antlr4::tree::ParseTree *target(ctx->expr());

	//Visit target 
	std::shared_ptr<ast> target_expr(visit(target).as<std::shared_ptr<ast>>());
	std::shared_ptr<ast_arith> target_expr_arith(std::static_pointer_cast<ast_arith>(target_expr));
	as->set_target(target_expr_arith);

	//No children left to visit
	return std::shared_ptr<ast>(as);
}

antlrcpp::Any parse_tree_translator::visitGapExpr(rtfssParser::GapExprContext *ctx){
	std::shared_ptr<ast> parent(ptp.get(ctx->parent));
	std::weak_ptr<ast> wparent(parent);

	std::shared_ptr<ast_gap> ag(std::make_shared<ast_gap>(wparent));
	ptp.put(ctx,ag);

	antlr4::tree::ParseTree *left(ctx->expr(0));
	antlr4::tree::ParseTree *right(ctx->expr(1));

	//Visit right and left expr
	std::shared_ptr<ast> left_expr(visit(left).as<std::shared_ptr<ast>>());
	std::shared_ptr<ast_arith> left_expr_arith(std::static_pointer_cast<ast_arith>(left_expr));
	std::shared_ptr<ast> right_expr(visit(right).as<std::shared_ptr<ast>>());
	std::shared_ptr<ast_arith> right_expr_arith(std::static_pointer_cast<ast_arith>(right_expr));

	ag->set_left(left_expr_arith);
	ag->set_right(right_expr_arith);

	//No children left to visit
	return std::shared_ptr<ast>(ag);
}

antlrcpp::Any parse_tree_translator::visitArrExpr(rtfssParser::ArrExprContext *ctx){

	std::shared_ptr<ast> parent(ptp.get(ctx->parent));
	std::weak_ptr<ast> wparent(parent);

	std::shared_ptr<ast_arrinit> aa(std::make_shared<ast_arrinit>(wparent));
	ptp.put(ctx,aa);

	std::vector<rtfssParser::ExprContext*> exprs(ctx->expr());

	//Visit all expr
	for(auto it=exprs.begin();it<exprs.end();it++){
		std::shared_ptr<ast> expr(visit(*it).as<std::shared_ptr<ast>>());
		std::shared_ptr<ast_arith> expr_arith(std::static_pointer_cast<ast_arith>(expr));
		aa->add_vals(expr_arith);
	}

	//No children left to visit
	return std::shared_ptr<ast>(aa);
}

antlrcpp::Any parse_tree_translator::visitDivExpr(rtfssParser::DivExprContext *ctx){
	std::shared_ptr<ast> parent(ptp.get(ctx->parent));
	std::weak_ptr<ast> wparent(parent);

	std::shared_ptr<ast_div> ad(std::make_shared<ast_div>(wparent));
	ptp.put(ctx,ad);

	antlr4::tree::ParseTree *left(ctx->expr(0));
	antlr4::tree::ParseTree *right(ctx->expr(1));

	//Visit right and left expr
	std::shared_ptr<ast> left_expr(visit(left).as<std::shared_ptr<ast>>());
	std::shared_ptr<ast_arith> left_expr_arith(std::static_pointer_cast<ast_arith>(left_expr));
	std::shared_ptr<ast> right_expr(visit(right).as<std::shared_ptr<ast>>());
	std::shared_ptr<ast_arith> right_expr_arith(std::static_pointer_cast<ast_arith>(right_expr));

	ad->set_left(left_expr_arith);
	ad->set_right(right_expr_arith);

	//No children left to visit
	return std::shared_ptr<ast>(ad);
}

antlrcpp::Any parse_tree_translator::visitEvalExpr(rtfssParser::EvalExprContext *ctx){
	std::shared_ptr<ast> parent(ptp.get(ctx->parent));
	std::weak_ptr<ast> wparent(parent);

	std::string operand(ctx->LOGIC_OP()->getText());
	std::shared_ptr<ast_comp> ac(std::make_shared<ast_comp>(wparent,operand));
	ptp.put(ctx,ac);

	antlr4::tree::ParseTree *left(ctx->expr(0));
	antlr4::tree::ParseTree *right(ctx->expr(1));

	//Visit right and left expr
	std::shared_ptr<ast> left_expr(visit(left).as<std::shared_ptr<ast>>());
	std::shared_ptr<ast_arith> left_expr_arith(std::static_pointer_cast<ast_arith>(left_expr));
	std::shared_ptr<ast> right_expr(visit(right).as<std::shared_ptr<ast>>());
	std::shared_ptr<ast_arith> right_expr_arith(std::static_pointer_cast<ast_arith>(right_expr));

	ac->set_left(left_expr_arith);
	ac->set_right(right_expr_arith);

	//No children left to visit
	return std::shared_ptr<ast>(ac);
}

antlrcpp::Any parse_tree_translator::visitVar_name(rtfssParser::Var_nameContext *ctx){
	std::shared_ptr<ast> parent(ptp.get(ctx->parent));
	std::weak_ptr<ast> wparent(parent);

	std::string name(ctx->IDENTIFIER()->getText());
	std::shared_ptr<ast_id> aid(std::make_shared<ast_id>(wparent,name));
	ptp.put(ctx,aid);

	//Visit index, if it exists
	if(ctx->expr()!=nullptr){
		std::shared_ptr<ast> index_expr(visit(ctx->expr()).as<std::shared_ptr<ast>>());
		std::shared_ptr<ast_arith> index_expr_arith(std::static_pointer_cast<ast_arith>(index_expr));

		aid->set_index(index_expr_arith);
	}

	//No children left to visit
	return std::shared_ptr<ast>(aid); 
}

antlrcpp::Any parse_tree_translator::visitStream_id(rtfssParser::Stream_idContext *ctx){
	std::shared_ptr<ast> parent(ptp.get(ctx->parent));
	std::weak_ptr<ast> wparent(parent);

	std::string name(ctx->IDENTIFIER()->getText());
	std::shared_ptr<ast_id> aid(std::make_shared<ast_id>(wparent,name));
	ptp.put(ctx,aid);

	//Visit index, if it exists
	if(ctx->idx!=nullptr){
		std::shared_ptr<ast> index_expr(visit(ctx->idx).as<std::shared_ptr<ast>>());
		std::shared_ptr<ast_arith> index_expr_arith(std::static_pointer_cast<ast_arith>(index_expr));

		aid->set_index(index_expr_arith);
	}

	//Visit instant, if it exists
	if(ctx->ins!=nullptr){
		std::shared_ptr<ast> ins_expr(visit(ctx->ins).as<std::shared_ptr<ast>>());
		std::shared_ptr<ast_arith> ins_expr_arith(std::static_pointer_cast<ast_arith>(ins_expr));

		aid->set_instant(ins_expr_arith);
	}

	//No children left to visit
	return std::shared_ptr<ast>(aid); 
}

//TODO MIDI Operators not implemented yet
antlrcpp::Any parse_tree_translator::visitNntofExpr(rtfssParser::NntofExprContext *ctx){
	//This function should never be called
	(void)ctx;
	error_handler::generic_not_implemented();
	return nullptr; 
}

antlrcpp::Any parse_tree_translator::visitCcofExpr(rtfssParser::CcofExprContext *ctx){
	//This function should never be called
	(void)ctx;
	error_handler::generic_not_implemented();
	return nullptr; 
}

antlrcpp::Any parse_tree_translator::visitNoteofExpr(rtfssParser::NoteofExprContext *ctx){
	//This function should never be called
	(void)ctx;
	error_handler::generic_not_implemented();
	return nullptr; 
}

antlrcpp::Any parse_tree_translator::visitVelofExpr(rtfssParser::VelofExprContext *ctx){
	//This function should never be called
	(void)ctx;
	error_handler::generic_not_implemented();
	return nullptr; 
}

antlrcpp::Any parse_tree_translator::visitPbendExpr(rtfssParser::PbendExprContext *ctx){
	//This function should never be called
	(void)ctx;
	error_handler::generic_not_implemented();
	return nullptr; 
}

antlrcpp::Any parse_tree_translator::visitCpofExpr(rtfssParser::CpofExprContext *ctx){
	//This function should never be called
	(void)ctx;
	error_handler::generic_not_implemented();
	return nullptr; 
}

antlrcpp::Any parse_tree_translator::visitPatofExpr(rtfssParser::PatofExprContext *ctx){
	//This function should never be called
	(void)ctx;
	error_handler::generic_not_implemented();
	return nullptr; 
}

antlrcpp::Any parse_tree_translator::visitPofExpr(rtfssParser::PofExprContext *ctx){
	//This function should never be called
	(void)ctx;
	error_handler::generic_not_implemented();
	return nullptr; 
}

antlrcpp::Any parse_tree_translator::visitFreqofExpr(rtfssParser::FreqofExprContext *ctx){
	//This function should never be called
	(void)ctx;
	error_handler::generic_not_implemented();
	return nullptr; 
}

//Other
antlrcpp::Any parse_tree_translator::visitData_type(rtfssParser::Data_typeContext *ctx){
	//This function should never be called
	(void)ctx;
	assert(false);
	return nullptr; 
}

antlrcpp::Any parse_tree_translator::visitFixedSizeType(rtfssParser::FixedSizeTypeContext *ctx){
	//This function should never be called
	(void)ctx;
	assert(false);
	return nullptr; 
}

antlrcpp::Any parse_tree_translator::visitVarSizeType(rtfssParser::VarSizeTypeContext *ctx){
	//This function should never be called
	(void)ctx;
	assert(false);
	return nullptr; 
}

antlrcpp::Any parse_tree_translator::visitPulse_freq(rtfssParser::Pulse_freqContext *ctx){
	//This function should never be called
	(void)ctx;
	assert(false);
	return nullptr; 
}
