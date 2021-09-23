#include "ast.h"

std::vector<std::shared_ptr<ast>> ast_start::get_children(){
	return code;
} 

std::vector<std::shared_ptr<ast>> ast_include::get_children(){
	return std::vector<std::shared_ptr<ast>>(); //No children/Terminal node
} 

std::vector<std::shared_ptr<ast>> ast_cblk_decl::get_children(){
	std::vector<std::shared_ptr<ast>> ret;

	//Add const_list
	ret.insert(ret.end(),const_list.begin(),const_list.end());

	//Add in_list
	ret.insert(ret.end(),in_list.begin(),in_list.end());

	//Add out_list
	ret.insert(ret.end(),out_list.begin(),out_list.end());

	//Add code_block
	ret.insert(ret.end(),code_block.begin(),code_block.end());

	return ret;
} 

std::vector<std::shared_ptr<ast>> ast_cblk_inst::get_children(){
	std::vector<std::shared_ptr<ast>> ret; 

	//Add const_list
	ret.insert(ret.end(),const_list.begin(),const_list.end());

	//Add in_list
	ret.insert(ret.end(),in_list.begin(),in_list.end());

	//Add out_list
	ret.insert(ret.end(),out_list.begin(),out_list.end());

	return ret;
} 

std::vector<std::shared_ptr<ast>> ast_s_cblk_inst::get_children(){
	std::vector<std::shared_ptr<ast>> ret; 

	//Add const_list
	ret.insert(ret.end(),const_list.begin(),const_list.end());

	//Add in_list
	ret.insert(ret.end(),in_list.begin(),in_list.end());

	return ret;
}	
std::vector<std::shared_ptr<ast>> ast_pulse::get_children(){
	return std::vector<std::shared_ptr<ast>>(); //No children/Terminal node
} 

std::vector<std::shared_ptr<ast>> ast_if::get_children(){
	std::vector<std::shared_ptr<ast>> ret; 

	//Add logic_expr
	ret.push_back(logic_expr);

	//Add code_block
	ret.insert(ret.end(),code_block.begin(),code_block.end());

	//Add elsif
	for(auto it=elseif.begin();it<elseif.end();it++){
		//Add elsif logic_expr
		ret.push_back(it->first);

		//Add elsif code_block
		ret.insert(ret.end(),it->second.begin(),it->second.end());
	}

	//Add else_code_block
	ret.insert(ret.end(),else_code_block.begin(),else_code_block.end());

	return ret;
}

std::vector<std::shared_ptr<ast>> ast_for::get_children(){
	std::vector<std::shared_ptr<ast>> ret; 

	//Add lower_bound
	ret.push_back(lower_bound);

	//Add upper_bound
	ret.push_back(upper_bound);

	if(increment!=nullptr){
		//Add increment
		ret.push_back(increment);
	}

	//Add for_code_block
	ret.insert(ret.end(),for_code_block.begin(),for_code_block.end());
	
	return ret;
}

std::vector<std::shared_ptr<ast>> ast_decl::get_children(){
	std::vector<std::shared_ptr<ast>> ret; 

	//Add var
	ret.push_back(var);

	if(attrib!=nullptr){
		//Add attrib
		ret.push_back(attrib);
	}

	return ret;
}

std::vector<std::shared_ptr<ast>> ast_assign::get_children(){
	std::vector<std::shared_ptr<ast>> ret; 

	//Add var
	ret.push_back(var);

	//Add attrib
	ret.push_back(attrib);

	return ret;
}	

std::vector<std::shared_ptr<ast>> ast_cast::get_children(){
	std::vector<std::shared_ptr<ast>> ret; 

	//Add target
	ret.push_back(target);

	return ret;
}

std::vector<std::shared_ptr<ast>> ast_arrinit::get_children(){
	std::vector<std::shared_ptr<ast>> ret; 

	//Add vals
	ret.insert(ret.end(),vals.begin(),vals.end());

	return ret;
}

std::vector<std::shared_ptr<ast>> ast_num::get_children(){
	return std::vector<std::shared_ptr<ast>>(); //No children/Terminal node
}

std::vector<std::shared_ptr<ast>> ast_id::get_children(){
	std::vector<std::shared_ptr<ast>> ret; 

	if(index!=nullptr){
		//Add index
		ret.push_back(index);
	}

	if(instant!=nullptr){
		//Add instant
		ret.push_back(instant);
	}

	return ret;
}	

std::vector<std::shared_ptr<ast>> ast_unary_arith::get_children(){
	std::vector<std::shared_ptr<ast>> ret; 

	//Add target
	ret.push_back(target);

	return ret;
}

std::vector<std::shared_ptr<ast>> ast_bin_arith::get_children(){
	std::vector<std::shared_ptr<ast>> ret; 

	//Add left
	ret.push_back(left);

	//Add right
	ret.push_back(right);

	return ret;
}
