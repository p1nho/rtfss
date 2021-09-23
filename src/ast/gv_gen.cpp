#include "ast.h"

unsigned int ast::vz_num=1;  //Node 0 reserved for orfans
std::string ast::vz_label("n");
bool ast::vz_show_parent(true);
bool ast::vz_orfan(false);
//std::map<std::weak_ptr<ast>,unsigned int,std::owner_less<std::weak_ptr<ast>>> ast::vz_parent;
std::map<ast*,unsigned int> ast::vz_parent;

unsigned int ast::gen_vz(std::stringstream &ss,bool top_level){
	//This function serves as a decorator to the funcion build_vz
	
	if(top_level){
		//TODO maybe reset vz_num?
		//Begin graph
		ss<<"digraph {"<<std::endl;
	}

	//Create base id
	unsigned int base=ast::vz_num++;

	if(ast::vz_show_parent){
		unsigned int par;
		auto find_par=ast::vz_parent.find(parent.lock().get());

		if(find_par!=ast::vz_parent.end())
			par=find_par->second;
		else{
			//std::cerr<<"Orfan node ptr="<<parent.lock().get()<<std::endl;
			par=0;

			//Create orfan node (0) if this is the first orfan
			if(!ast::vz_orfan){
				ast::vz_orfan=true;
				ss<<ast::vz_label<<0<<" [ "
					<<"label="<<"orfan"
					<<"; style=dashed"
					<<"; color=red"
					<<"; fontcolor=red"
					<<" ]"<<std::endl;
			}

		}

		//Base - Parent
		ss<<ast::vz_label<<base<<" -> "
			<<ast::vz_label<<par<<" [ "
			<<"color=red; style=dashed"
			<<" ]"<<std::endl;

		//Add base id to vz_parent
		ast::vz_parent.insert(std::make_pair(this,base));
	}

	//Call build_vz
	build_vz(ss,base);

	if(ast::vz_show_parent){
		//Remove base id from vz_parent
		ast::vz_parent.erase(this); 

		//Set orfans flag off if map is empty
		if(ast::vz_parent.size()==0){
			ast::vz_orfan=false;
		}
	}

	if(top_level){
		//End graph
		ss<<"}"<<std::endl;
	}

	return base;
}



void ast_id::build_vz(std::stringstream &ss,unsigned int base){
	//Not Terminal 
	unsigned int name=ast::vz_num++;
	unsigned int dt=ast::vz_num++;
	unsigned int pid=ast::vz_num++;

	//Create Nodes
	//Base
	ss<<ast::vz_label<<base<<" [ "
		<<"label="<<"id"
		<<"; style=bold"
		<<" ]"<<std::endl;

	//Name
	ss<<ast::vz_label<<name<<" [ "
		<<"label="<<"\""<<this->name<<"\""
		<<" ]"<<std::endl;

	//Base - Name
	ss<<ast::vz_label<<base<<" -> "
		<<ast::vz_label<<name<<" [ "
		<<"label="<<"name"
		<<" ]"<<std::endl;
	
	//Datatype
	std::string dts(this->dt.to_string());
	if(dts.length()!=0){
		ss<<ast::vz_label<<dt<<" [ "
			<<"label="<<"\""<<dts<<"\""
			<<" ]"<<std::endl;
	}

	//Base - Datatype
	if(dts.length()!=0){
		ss<<ast::vz_label<<base<<" -> "
			<<ast::vz_label<<dt<<" [ "
			<<"label="<<"datatype"
			<<" ]"<<std::endl;
	}
	
	//Pulse
	std::string pids(this->pid.to_string());
	if(pids.length()!=0){
		ss<<ast::vz_label<<pid<<" [ "
			<<"label="<<"\""<<pids<<"\""
			<<" ]"<<std::endl;
	}

	//Base - Pulse
	if(pids.length()!=0){
		ss<<ast::vz_label<<base<<" -> "
			<<ast::vz_label<<pid<<" [ "
			<<"label="<<"pulse"
			<<" ]"<<std::endl;
	}

	//Get index, if available
	if(this->index!=nullptr){
		unsigned int index=this->index->gen_vz(ss,false);
		
		ss<<ast::vz_label<<base<<" -> "
			<<ast::vz_label<<index<<" [ "
			<<"label="<<"index"
			<<" ]"<<std::endl;
	}

	//Get instant, if available
	if(this->instant!=nullptr){
		unsigned int instant=this->instant->gen_vz(ss,false);
		
		ss<<ast::vz_label<<base<<" -> "
			<<ast::vz_label<<instant<<" [ "
			<<"label="<<"instant"
			<<" ]"<<std::endl;
	}

}

void ast_start::build_vz(std::stringstream &ss,unsigned int base){
	//Non Terminal
	//Start rule

	//Base
	ss<<ast::vz_label<<base<<" [ "
		<<"label="<<"\""<<file_name<<"\""
		<<"; style=bold"
		<<" ]"<<std::endl;

	//For each code node
	for(auto it=code.begin();it<code.end();it++){
		//Call build_vz
		unsigned int ret=(*it)->gen_vz(ss,false);

		//Connect to base
		ss<<ast::vz_label<<base<<" -> "
			<<ast::vz_label<<ret<<" [ "
			<<"label="<<"\""<<"code"<<"\""
			<<" ]"<<std::endl;
	}

}

void ast_include::build_vz(std::stringstream &ss,unsigned int base){
	//Terminal
	unsigned int file_name=ast::vz_num++;

	//Base
	ss<<ast::vz_label<<base<<" [ "
		<<"label="<<"\""<<"use"<<"\""
		<<"; style=bold"
		<<" ]"<<std::endl;

	//File
	ss<<ast::vz_label<<file_name<<" [ "
		<<"label="<<"\""<<this->file_name<<"\""
		<<" ]"<<std::endl;
	
	//Base - File
	ss<<ast::vz_label<<base<<" -> "
		<<ast::vz_label<<file_name<<" [ "
		<<"label="<<"\""<<"file"<<"\""
		<<" ]"<<std::endl;
	
} 
void ast_cblk_decl::build_vz(std::stringstream &ss,unsigned int base){
	//Non Terminal
	unsigned int name=ast::vz_num++;
	unsigned int pid=ast::vz_num++;

	//Base
	ss<<ast::vz_label<<base<<" [ "
		<<"label="<<"\""<<"cblk_decl"<<"\""
		<<"; style=bold"
		<<" ]"<<std::endl;

	//Name
	ss<<ast::vz_label<<name<<" [ "
		<<"label="<<"\""<<this->cblock_name<<"\""
		<<" ]"<<std::endl;

	//Pulse
	ss<<ast::vz_label<<pid<<" [ "
		<<"label="<<"\""<<this->pid.to_string()<<"\""
		<<" ]"<<std::endl;

	//Base - Name
	ss<<ast::vz_label<<base<<" -> "
		<<ast::vz_label<<name<<" [ "
		<<"label="<<"\""<<"name"<<"\""
		<<" ]"<<std::endl;

	//Base - Pulse
	ss<<ast::vz_label<<base<<" -> "
		<<ast::vz_label<<pid<<" [ "
		<<"label="<<"\""<<"pulse"<<"\""
		<<" ]"<<std::endl;

	//For each const node
	for(auto it=const_list.begin();it<const_list.end();it++){
		//Call build_vz
		unsigned int ret=(*it)->gen_vz(ss,false);

		//Connect to base
		ss<<ast::vz_label<<base<<" -> "
			<<ast::vz_label<<ret<<" [ "
			<<"label="<<"\""<<"const"<<"\""
			<<" ]"<<std::endl;
	}

	//For each in node
	for(auto it=in_list.begin();it<in_list.end();it++){
		//Call build_vz
		unsigned int ret=(*it)->gen_vz(ss,false);

		//Connect to base
		ss<<ast::vz_label<<base<<" -> "
			<<ast::vz_label<<ret<<" [ "
			<<"label="<<"\""<<"in"<<"\""
			<<" ]"<<std::endl;
	}

	//For each out node
	for(auto it=out_list.begin();it<out_list.end();it++){
		//Call build_vz
		unsigned int ret=(*it)->gen_vz(ss,false);

		//Connect to base
		ss<<ast::vz_label<<base<<" -> "
			<<ast::vz_label<<ret<<" [ "
			<<"label="<<"\""<<"out"<<"\""
			<<" ]"<<std::endl;
	}

	//For each code node
	for(auto it=code_block.begin();it<code_block.end();it++){
		//Call build_vz
		unsigned int ret=(*it)->gen_vz(ss,false);

		//Connect to base
		ss<<ast::vz_label<<base<<" -> "
			<<ast::vz_label<<ret<<" [ "
			<<"label="<<"\""<<"code"<<"\""
			<<" ]"<<std::endl;
	}

}
void ast_s_cblk_inst::build_vz(std::stringstream &ss,unsigned int base){
	//Non Terminal
	unsigned int name=ast::vz_num++;
	unsigned int pid=ast::vz_num++;

	//Base
	ss<<ast::vz_label<<base<<" [ "
		<<"label="<<"\""<<"s_cblk_inst"<<"\""
		<<"; style=bold"
		<<" ]"<<std::endl;

	//Name
	ss<<ast::vz_label<<name<<" [ "
		<<"label="<<"\""<<this->cblock_name<<"\""
		<<" ]"<<std::endl;

	//Pulse
	ss<<ast::vz_label<<pid<<" [ "
		<<"label="<<"\""<<this->pid.to_string()<<"\""
		<<" ]"<<std::endl;

	//Base - Name
	ss<<ast::vz_label<<base<<" -> "
		<<ast::vz_label<<name<<" [ "
		<<"label="<<"\""<<"name"<<"\""
		<<" ]"<<std::endl;

	//Base - Pulse
	ss<<ast::vz_label<<base<<" -> "
		<<ast::vz_label<<pid<<" [ "
		<<"label="<<"\""<<"pulse"<<"\""
		<<" ]"<<std::endl;

	//For each const node
	for(auto it=const_list.begin();it<const_list.end();it++){
		//Call build_vz
		unsigned int ret=(*it)->gen_vz(ss,false);

		//Connect to base
		ss<<ast::vz_label<<base<<" -> "
			<<ast::vz_label<<ret<<" [ "
			<<"label="<<"\""<<"const"<<"\""
			<<" ]"<<std::endl;
	}

	//For each in node
	for(auto it=in_list.begin();it<in_list.end();it++){
		//Call build_vz
		unsigned int ret=(*it)->gen_vz(ss,false);

		//Connect to base
		ss<<ast::vz_label<<base<<" -> "
			<<ast::vz_label<<ret<<" [ "
			<<"label="<<"\""<<"in"<<"\""
			<<" ]"<<std::endl;
	}
}

void ast_cblk_inst::build_vz(std::stringstream &ss,unsigned int base){
	//Non Terminal
	unsigned int name=ast::vz_num++;
	unsigned int pid=ast::vz_num++;

	//Base
	ss<<ast::vz_label<<base<<" [ "
		<<"label="<<"\""<<"cblk_inst"<<"\""
		<<"; style=bold"
		<<" ]"<<std::endl;

	//Name
	ss<<ast::vz_label<<name<<" [ "
		<<"label="<<"\""<<this->cblock_name<<"\""
		<<" ]"<<std::endl;

	//Pulse
	ss<<ast::vz_label<<pid<<" [ "
		<<"label="<<"\""<<this->pid.to_string()<<"\""
		<<" ]"<<std::endl;

	//Base - Name
	ss<<ast::vz_label<<base<<" -> "
		<<ast::vz_label<<name<<" [ "
		<<"label="<<"\""<<"name"<<"\""
		<<" ]"<<std::endl;

	//Base - Pulse
	ss<<ast::vz_label<<base<<" -> "
		<<ast::vz_label<<pid<<" [ "
		<<"label="<<"\""<<"pulse"<<"\""
		<<" ]"<<std::endl;

	//For each const node
	for(auto it=const_list.begin();it<const_list.end();it++){
		//Call build_vz
		unsigned int ret=(*it)->gen_vz(ss,false);

		//Connect to base
		ss<<ast::vz_label<<base<<" -> "
			<<ast::vz_label<<ret<<" [ "
			<<"label="<<"\""<<"const"<<"\""
			<<" ]"<<std::endl;
	}

	//For each in node
	for(auto it=in_list.begin();it<in_list.end();it++){
		//Call build_vz
		unsigned int ret=(*it)->gen_vz(ss,false);

		//Connect to base
		ss<<ast::vz_label<<base<<" -> "
			<<ast::vz_label<<ret<<" [ "
			<<"label="<<"\""<<"in"<<"\""
			<<" ]"<<std::endl;
	}

	//For each out node
	for(auto it=out_list.begin();it<out_list.end();it++){
		//Call build_vz
		unsigned int ret=(*it)->gen_vz(ss,false);

		//Connect to base
		ss<<ast::vz_label<<base<<" -> "
			<<ast::vz_label<<ret<<" [ "
			<<"label="<<"\""<<"out"<<"\""
			<<" ]"<<std::endl;
	}

}
void ast_pulse::build_vz(std::stringstream &ss,unsigned int base){
	//Terminal
	unsigned int pid=ast::vz_num++;
	unsigned int val=ast::vz_num++;
	unsigned int scale=ast::vz_num++;

	//Base
	ss<<ast::vz_label<<base<<" [ "
		<<"label="<<"\""<<"pulse_decl"<<"\""
		<<"; style=bold"
		<<" ]"<<std::endl;
	
	//Pulse id
	ss<<ast::vz_label<<pid<<" [ "
		<<"label="<<"\""<<this->pid.to_string()<<"\""
		<<" ]"<<std::endl;

	//Val
	ss<<ast::vz_label<<val<<" [ "
		<<"label="<<"\""<<this->val<<"\""
		<<" ]"<<std::endl;

	//Scale
	ss<<ast::vz_label<<scale<<" [ "
		<<"label="<<"\""<<ts.to_string()<<"\""
		<<" ]"<<std::endl;

	//Base - Pid
	ss<<ast::vz_label<<base<<" -> "
		<<ast::vz_label<<pid<<" [ "
		<<"label="<<"\""<<"name"<<"\""
		<<" ]"<<std::endl;

	//Base - Val
	ss<<ast::vz_label<<base<<" -> "
		<<ast::vz_label<<val<<" [ "
		<<"label="<<"\""<<"value"<<"\""
		<<" ]"<<std::endl;

	//Base - Timescale
	ss<<ast::vz_label<<base<<" -> "
		<<ast::vz_label<<scale<<" [ "
		<<"label="<<"\""<<"time_scale"<<"\""
		<<" ]"<<std::endl;

}

void ast_if::build_vz(std::stringstream &ss,unsigned int base){ 
	//Non Terminal

	//Base
	ss<<ast::vz_label<<base<<" [ "
		<<"label="<<"\""<<"if"<<"\""
		<<"; style=bold"
		<<" ]"<<std::endl;

	//Get logic_expr
	unsigned int logic_expr=this->logic_expr->gen_vz(ss,false);

	//Base - logic_expr
	ss<<ast::vz_label<<base<<" -> "
		<<ast::vz_label<<logic_expr<<" [ "
		<<"label="<<"\""<<"if_logic"<<"\""
		<<" ]"<<std::endl;

	//For each code node
	for(auto it=code_block.begin();it<code_block.end();it++){
		//Call build_vz
		unsigned int ret=(*it)->gen_vz(ss,false);

		//Connect to base
		ss<<ast::vz_label<<base<<" -> "
			<<ast::vz_label<<ret<<" [ "
			<<"label="<<"\""<<"if_true"<<"\""
			<<" ]"<<std::endl;
	}

	//For each elsif node
	unsigned int elseif_cnt=0;
	for(auto it=elseif.begin();it<elseif.end();it++){
		unsigned int curr=elseif_cnt++;
		
		//Get logic_expr
		unsigned int le=(*it).first->gen_vz(ss,false);

		//Connect to base
		ss<<ast::vz_label<<base<<" -> "
			<<ast::vz_label<<le<<" [ "
			<<"label="<<"\""<<"elseif"<<curr<<"_logic"<<"\""
			<<" ]"<<std::endl;


		//For each code node
		std::vector<std::shared_ptr<ast>> &elseif_code((*it).second);
		for(auto it=elseif_code.begin();it<elseif_code.end();it++){
			//Call build_vz
			unsigned int ret=(*it)->gen_vz(ss,false);

			//Connect to base
			ss<<ast::vz_label<<base<<" -> "
				<<ast::vz_label<<ret<<" [ "
				<<"label="<<"\""<<"elseif"<<curr<<"_code"<<"\""
				<<" ]"<<std::endl;
		}
	}

	//For each else_code node
	for(auto it=else_code_block.begin();it<else_code_block.end();it++){
		//Call build_vz
		unsigned int ret=(*it)->gen_vz(ss,false);

		//Connect to base
		ss<<ast::vz_label<<base<<" -> "
			<<ast::vz_label<<ret<<" [ "
			<<"label="<<"\""<<"else_code"<<"\""
			<<" ]"<<std::endl;
	}
	
}

void ast_for::build_vz(std::stringstream &ss,unsigned int base){
	//Non Terminal
	unsigned int for_var=ast::vz_num++;

	//Base
	ss<<ast::vz_label<<base<<" [ "
		<<"label="<<"\""<<"for"<<"\""
		<<"; style=bold"
		<<" ]"<<std::endl;

	//For Var
	ss<<ast::vz_label<<for_var<<" [ "
		<<"label="<<"\""<<this->for_var<<"\""
		<<" ]"<<std::endl;

	//Base - For var
	ss<<ast::vz_label<<base<<" -> "
		<<ast::vz_label<<for_var<<" [ "
		<<"label="<<"\""<<"for_var"<<"\""
		<<" ]"<<std::endl;

	//Get lower bound
	unsigned int lower = lower_bound->gen_vz(ss,false);

	//Base - lower bound
	ss<<ast::vz_label<<base<<" -> "
		<<ast::vz_label<<lower<<" [ "
		<<"label="<<"\""<<"lowerb"<<"\""
		<<" ]"<<std::endl;

	//Get upper bound
	unsigned int upper = upper_bound->gen_vz(ss,false);

	//Base - upper bound
	ss<<ast::vz_label<<base<<" -> "
		<<ast::vz_label<<upper<<" [ "
		<<"label="<<"\""<<"upperb"<<"\""
		<<" ]"<<std::endl;

	//Get increment, if it exists
	if(increment!=nullptr){
		unsigned int inc = increment->gen_vz(ss,false);

		//Base - increment 
		ss<<ast::vz_label<<base<<" -> "
			<<ast::vz_label<<inc<<" [ "
			<<"label="<<"\""<<"inc"<<"\""
			<<" ]"<<std::endl;
	}


	//For each code node
	for(auto it=for_code_block.begin();it<for_code_block.end();it++){
		//Call build_vz
		unsigned int ret=(*it)->gen_vz(ss,false);

		//Connect to base
		ss<<ast::vz_label<<base<<" -> "
			<<ast::vz_label<<ret<<" [ "
			<<"label="<<"\""<<"code"<<"\""
			<<" ]"<<std::endl;
	}

}

void ast_decl::build_vz(std::stringstream &ss,unsigned int base){
	//Non Terminal

	//Base
	ss<<ast::vz_label<<base<<" [ "
		<<"label="<<"\""<<"data_decl"<<"\""
		<<"; style=bold"
		<<" ]"<<std::endl;


	//Get var
	unsigned int bvar=var->gen_vz(ss,false);

	//Base - Var
	ss<<ast::vz_label<<base<<" -> "
		<<ast::vz_label<<bvar<<" [ "
		<<"label="<<"\""<<"var"<<"\""
		<<" ]"<<std::endl;

	if(attrib!=nullptr){
		unsigned int battrib=attrib->gen_vz(ss,false);

		//Base - Attrib
		ss<<ast::vz_label<<base<<" -> "
			<<ast::vz_label<<battrib<<" [ "
			<<"label="<<"\""<<"attrib"<<"\""
			<<" ]"<<std::endl;
	}

}

void ast_assign::build_vz(std::stringstream &ss,unsigned int base){
	//Non Terminal
	unsigned int ao=ast::vz_num++;

	//Base
	ss<<ast::vz_label<<base<<" [ "
		<<"label="<<"\""<<"data_assign"<<"\""
		<<"; style=bold"
		<<" ]"<<std::endl;


	//Get var
	unsigned int bvar=var->gen_vz(ss,false);

	//Assign op
	ss<<ast::vz_label<<ao<<" [ "
		<<"label="<<"\""<<this->ao.to_string()<<"\""
		<<" ]"<<std::endl;
	
	//Get attrib
	unsigned int battrib=attrib->gen_vz(ss,false);


	//Base - Var
	ss<<ast::vz_label<<base<<" -> "
		<<ast::vz_label<<bvar<<" [ "
		<<"label="<<"\""<<"var"<<"\""
		<<" ]"<<std::endl;

	//Base - Assignop
	ss<<ast::vz_label<<base<<" -> "
		<<ast::vz_label<<ao<<" [ "
		<<"label="<<"\""<<"op"<<"\""
		<<" ]"<<std::endl;

	//Base - Attrib
	ss<<ast::vz_label<<base<<" -> "
		<<ast::vz_label<<battrib<<" [ "
		<<"label="<<"\""<<"attrib"<<"\""
		<<" ]"<<std::endl;

}

void ast_unary_arith::unary_build_vz(std::stringstream &ss,unsigned int base,std::string op){
	//Non Terminal

	//Base
	ss<<ast::vz_label<<base<<" [ "
		<<"label="<<"\""<<op<<"\""
		<<"; style=bold"
		<<" ]"<<std::endl;
	
	//Get target
	unsigned int target=this->target->gen_vz(ss,false);

	//Base - Target
	ss<<ast::vz_label<<base<<" -> "
		<<ast::vz_label<<target<<" [ "
		<<"label="<<"\""<<"target"<<"\""
		<<" ]"<<std::endl;

}

void ast_bin_arith::bin_build_vz(std::stringstream &ss,unsigned int base,std::string op){
	//Non Terminal

	//Base
	ss<<ast::vz_label<<base<<" [ "
		<<"label="<<"\""<<op<<"\""
		<<"; style=bold"
		<<" ]"<<std::endl;
	
	//Get left
	unsigned int left=this->left->gen_vz(ss,false);

	//Base - Left
	ss<<ast::vz_label<<base<<" -> "
		<<ast::vz_label<<left<<" [ "
		<<"label="<<"\""<<"left"<<"\""
		<<" ]"<<std::endl;

	//Get right
	unsigned int right=this->right->gen_vz(ss,false);

	//Base - Right
	ss<<ast::vz_label<<base<<" -> "
		<<ast::vz_label<<right<<" [ "
		<<"label="<<"\""<<"right"<<"\""
		<<" ]"<<std::endl;

}

void ast_add::build_vz(std::stringstream &ss,unsigned int base){
	//Non Terminal
	return bin_build_vz(ss,base,"+");
}

void ast_sub::build_vz(std::stringstream &ss,unsigned int base){
	//Non Terminal
	return bin_build_vz(ss,base,"-");
}

void ast_sim::build_vz(std::stringstream &ss,unsigned int base){
	//Non Terminal
	return unary_build_vz(ss,base,"-");
}

void ast_mult::build_vz(std::stringstream &ss,unsigned int base){
	//Non Terminal
	return bin_build_vz(ss,base,"*");
}

void ast_div::build_vz(std::stringstream &ss,unsigned int base){
	//Non Terminal
	return bin_build_vz(ss,base,"/");
}

void ast_shl::build_vz(std::stringstream &ss,unsigned int base){
	//Non Terminal
	return bin_build_vz(ss,base,"<<");
}

void ast_shr::build_vz(std::stringstream &ss,unsigned int base){
	//Non Terminal
	return bin_build_vz(ss,base,">>");
}

void ast_rtl::build_vz(std::stringstream &ss,unsigned int base){
	//Non Terminal
	return bin_build_vz(ss,base,"<<<");
}

void ast_rtr::build_vz(std::stringstream &ss,unsigned int base){
	//Non Terminal
	return bin_build_vz(ss,base,">>>");
}

void ast_and::build_vz(std::stringstream &ss,unsigned int base){
	//Non Terminal
	return bin_build_vz(ss,base,"and");
}

void ast_or::build_vz(std::stringstream &ss,unsigned int base){
	//Non Terminal
	return bin_build_vz(ss,base,"or");
}

void ast_xor::build_vz(std::stringstream &ss,unsigned int base){
	//Non Terminal
	return bin_build_vz(ss,base,"xor");
}

void ast_not::build_vz(std::stringstream &ss,unsigned int base){
	//Non Terminal
	return unary_build_vz(ss,base,"not");
}

void ast_gap::build_vz(std::stringstream &ss,unsigned int base){
	//Non Terminal
	return bin_build_vz(ss,base,"gap");
}

void ast_comp::build_vz(std::stringstream &ss,unsigned int base){
	//Non Terminal
	return bin_build_vz(ss,base,op.to_string());
}

void ast_prop::build_vz(std::stringstream &ss,unsigned int base){
	//Non Terminal
	return unary_build_vz(ss,base,"&");
}

void ast_mod::build_vz(std::stringstream &ss,unsigned int base){
	//Non Terminal
	return bin_build_vz(ss,base,"%");
}

void ast_cast::build_vz(std::stringstream &ss,unsigned int base){
	//Non Terminal
	unsigned int dt=ast::vz_num++;

	//Base
	ss<<ast::vz_label<<base<<" [ "
		<<"label="<<"\""<<"cast"<<"\""
		<<"; style=bold"
		<<" ]"<<std::endl;

	//Datatype
	ss<<ast::vz_label<<dt<<" [ "
		<<"label="<<"\""<<this->dt.to_string()<<"\""
		<<" ]"<<std::endl;

	//Target	
	unsigned int target=this->target->gen_vz(ss,false);

	//Base - Datatype
	ss<<ast::vz_label<<base<<" -> "
		<<ast::vz_label<<dt<<" [ "
		<<"label="<<"\""<<"type"<<"\""
		<<" ]"<<std::endl;

	//Base - Target
	ss<<ast::vz_label<<base<<" -> "
		<<ast::vz_label<<target<<" [ "
		<<"label="<<"\""<<"target"<<"\""
		<<" ]"<<std::endl;
	
}

void ast_arrinit::build_vz(std::stringstream &ss,unsigned int base){
	//Non Terminal

	//Base
	ss<<ast::vz_label<<base<<" [ "
		<<"label="<<"\""<<"const_arr"<<"\""
		<<"; style=bold"
		<<" ]"<<std::endl;

	//For each arith node
	for(auto it=vals.begin();it<vals.end();it++){
		//Call build_vz
		unsigned int ret=(*it)->gen_vz(ss,false);

		//Connect to base
		ss<<ast::vz_label<<base<<" -> "
			<<ast::vz_label<<ret<<" [ "
			<<"label="<<"\""<<"val"<<"\""
			<<" ]"<<std::endl;
	}

}

void ast_num::build_vz(std::stringstream &ss,unsigned int base){
	//Terminal

	//Base
	ss<<ast::vz_label<<base<<" [ "
		<<"label="<<"\"";
	
	if(is_int)
		ss<<i;
	else
		ss<<d;

	ss<<"\""
		<<"; style=bold"
		<<" ]"<<std::endl;

}
