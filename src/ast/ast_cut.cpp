#include "ast.h"

std::shared_ptr<ast> ast_start::cut_child(std::shared_ptr<ast> child){
	//Search on code
	auto target(std::find(code.begin(),code.end(),child));
	if(target!=code.end()){ //Found child
		std::shared_ptr<ast> child(*target);
		code.erase(target);
		return child;
	}

	return nullptr;
}          

std::shared_ptr<ast> ast_include::cut_child(std::shared_ptr<ast> child){
	//No children nodes
	(void) child;
	return nullptr;
}        

std::shared_ptr<ast> ast_cblk_decl::cut_child(std::shared_ptr<ast> child){
	//Search on const_list
	{
		auto target(std::find(const_list.begin(),const_list.end(),child));
		if(target!=const_list.end()){ //Found child
			std::shared_ptr<ast> child(*target);
			const_list.erase(target);
			return child;
		}
	}

	//Search on in_list
	{
		auto target(std::find(in_list.begin(),in_list.end(),child));
		if(target!=in_list.end()){ //Found child
			std::shared_ptr<ast> child(*target);
			in_list.erase(target);
			return child;
		}
	}

	//Search on out_list
	{
		auto target(std::find(out_list.begin(),out_list.end(),child));
		if(target!=out_list.end()){ //Found child
			std::shared_ptr<ast> child(*target);
			out_list.erase(target);
			return child;
		}
	}

	//Search on code_block
	{
		auto target(std::find(code_block.begin(),code_block.end(),child));
		if(target!=code_block.end()){ //Found child
			std::shared_ptr<ast> child(*target);
			code_block.erase(target);
			return child;
		}
	}

	return nullptr;
}      

std::shared_ptr<ast> ast_cblk_inst::cut_child(std::shared_ptr<ast> child){
	//Search on const_list
	{
		auto target(std::find(const_list.begin(),const_list.end(),child));
		if(target!=const_list.end()){ //Found child
			std::shared_ptr<ast> child(*target);
			const_list.erase(target);
			return child;
		}
	}

	//Search on in_list
	{
		auto target(std::find(in_list.begin(),in_list.end(),child));
		if(target!=in_list.end()){ //Found child
			std::shared_ptr<ast> child(*target);
			in_list.erase(target);
			return child;
		}
	}

	//Search on out_list
	{
		auto target(std::find(out_list.begin(),out_list.end(),child));
		if(target!=out_list.end()){ //Found child
			std::shared_ptr<ast> child(*target);
			out_list.erase(target);
			return child;
		}
	}

	return nullptr;
}      

std::shared_ptr<ast> ast_s_cblk_inst::cut_child(std::shared_ptr<ast> child){
	//Search on const_list
	{
		auto target(std::find(const_list.begin(),const_list.end(),child));
		if(target!=const_list.end()){ //Found child
			std::shared_ptr<ast> child(*target);
			const_list.erase(target);
			return child;
		}
	}

	//Search on in_list
	{
		auto target(std::find(in_list.begin(),in_list.end(),child));
		if(target!=in_list.end()){ //Found child
			std::shared_ptr<ast> child(*target);
			in_list.erase(target);
			return child;
		}
	}

	return nullptr;
}    

std::shared_ptr<ast> ast_pulse::cut_child(std::shared_ptr<ast> child){
	//No children nodes
	(void) child;
	return nullptr;
}          

std::shared_ptr<ast> ast_if::cut_child(std::shared_ptr<ast> child){
	//Check if it is logic_expr
	if(child==logic_expr){
		logic_expr=nullptr;
		return child; 
	}

	//Search on code_block
	{
		auto target(std::find(code_block.begin(),code_block.end(),child));
		if(target!=code_block.end()){ //Found child
			std::shared_ptr<ast> child(*target);
			code_block.erase(target);
			return child;
		}
	}

	//Search on elseif
	for(auto it=elseif.begin();it!=elseif.end();it++){

		//Check if it is elseif_logic_expr
		if(child==it->first){
			it->first=nullptr;
			return child; 
		}

		//Check elseif_code_block
		auto target(std::find(it->second.begin(),it->second.end(),child));
		if(target!=it->second.end()){ //Found child
			std::shared_ptr<ast> child(*target);
			it->second.erase(target);
			return child;
		}
	}	

	//Search on else_code_block
	{
		auto target(std::find(else_code_block.begin(),else_code_block.end(),child));
		if(target!=else_code_block.end()){ //Found child
			std::shared_ptr<ast> child(*target);
			else_code_block.erase(target);
			return child;
		}
	}

	return nullptr;
}             

std::shared_ptr<ast> ast_for::cut_child(std::shared_ptr<ast> child){
	//Check if it is lower_bound
	if(child==lower_bound){
		lower_bound=nullptr;
		return child; 
	}

	//Check if it is upper_bound
	if(child==upper_bound){
		upper_bound=nullptr;
		return child; 
	}

	//Check if it is increment
	if(child==increment){
		increment=nullptr;
		return child; 
	}

	//Search on for_code_block
	{
		auto target(std::find(for_code_block.begin(),for_code_block.end(),child));
		if(target!=for_code_block.end()){ //Found child
			std::shared_ptr<ast> child(*target);
			for_code_block.erase(target);
			return child;
		}
	}

	return nullptr;
}            

std::shared_ptr<ast> ast_decl::cut_child(std::shared_ptr<ast> child){
	//Check if it is var
	if(child==var){
		var=nullptr;
		return child; 
	}

	//Check if it is attrib
	if(child==attrib){
		attrib=nullptr;
		return child; 
	}

	return nullptr;
}           

std::shared_ptr<ast> ast_assign::cut_child(std::shared_ptr<ast> child){
	//Check if it is var
	if(child==var){
		var=nullptr;
		return child; 
	}

	//Check if it is attrib
	if(child==attrib){
		attrib=nullptr;
		return child; 
	}

	return nullptr;
}         

std::shared_ptr<ast> ast_cast::cut_child(std::shared_ptr<ast> child){
	//Check if it is target
	if(child==target){
		target=nullptr;
		return child; 
	}

	return nullptr;
}           

std::shared_ptr<ast> ast_arrinit::cut_child(std::shared_ptr<ast> child){
	//Search on vals
	{
		auto target(std::find(vals.begin(),vals.end(),child));
		if(target!=vals.end()){ //Found child
			std::shared_ptr<ast> child(*target);
			vals.erase(target);
			return child;
		}
	}
	return nullptr;
}        

std::shared_ptr<ast> ast_num::cut_child(std::shared_ptr<ast> child){
	//No children nodes
	(void) child;
	return nullptr;
}            

std::shared_ptr<ast> ast_id::cut_child(std::shared_ptr<ast> child){
	//No children nodes
	(void) child;
	return nullptr;
}   


std::shared_ptr<ast> ast_unary_arith::cut_child(std::shared_ptr<ast> child){
	//Check if it is target
	if(child==target){
		target=nullptr;
		return child; 
	}

	return nullptr;
}   

std::shared_ptr<ast> ast_bin_arith::cut_child(std::shared_ptr<ast> child){
	//Check if it is left
	if(child==left){
		left=nullptr;
		return child; 
	}

	//Check if it is attrib
	if(child==right){
		right=nullptr;
		return child; 
	}

	return nullptr;
}   
