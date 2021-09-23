#include "arch_node.h"
#include <boost/dynamic_bitset/dynamic_bitset.hpp>

std::string arch_node::gv_label("n");

//arch_node
arch_node::arch_op arch_node::get_op(){
	return op;
}

unsigned int arch_node::get_node_id(){
	return node_id;
}

unsigned int arch_node::get_pulse_id(){
	return pulse_id;
}

datatype arch_node::get_datatype(){
	return dt;
}

void arch_node::set_datatype(datatype dt){
	this->dt=dt;
}

void arch_node::gz_extra_conn(std::stringstream &ss){
	//No extra connections on the base implementations
	(void) ss;
}

//arch_unary_arith
void arch_unary_arith::gz_extra_conn(std::stringstream &ss){
	//No extra connections on the base implementations
	(void) ss;
	/*
	//Add target connection
	ss<<gv_label<<get_node_id()<<" -> "
		<<gv_label<<target->get_node_id()<<" [ "
		<<"label="<<"target; "
		<<"fontcolor=red;"
		<<"color=red; style=dashed"
		<<" ]"<<std::endl;
		TODO remove for real
	*/
}

//arch_unary_arith
void arch_bin_arith::gz_extra_conn(std::stringstream &ss){
	//Add right connection
	ss<<gv_label<<get_node_id()<<" -> "
		<<gv_label<<right->get_node_id()<<" [ "
		<<"label="<<"right; "
		<<"fontcolor=red;"
		<<"color=red; style=dashed"
		<<" ]"<<std::endl;

	//Add left connection
	ss<<gv_label<<get_node_id()<<" -> "
		<<gv_label<<left->get_node_id()<<" [ "
		<<"label="<<"left; "
		<<"fontcolor=red;"
		<<"color=red; style=dashed"
		<<" ]"<<std::endl;
}

//arch_id
std::string arch_id::get_name(){
	return name;
}

std::string arch_id::get_cblock_name(){
	return cblock_name;
}

var_property::var_kind arch_id::get_vk(){
	return vk;
}

//arch_ins
void arch_ins::set_default_value(boost::dynamic_bitset<> dv){
	default_value=dv;
}

std::string arch_ins::get_name(){
	return name;
}

unsigned int arch_ins::get_inst(){
	return inst;
}

boost::dynamic_bitset<> arch_ins::get_default_value(){
	return default_value;
}

std::string arch_ins::get_df_in_hex(){
    char hex[] = "0123456789ABCDEF";
    std::string num;

    for(unsigned long i=0;i<std::ceil(default_value.size()/4.0);i++){
        num=hex[((default_value>>(i*4))&boost::dynamic_bitset<>(default_value.size(),0xF)).to_ulong()]+num;
    }

	return num;
}

var_property::var_kind arch_ins::get_vk(){
	return vk;
}

void arch_ins::gz_extra_conn(std::stringstream &ss){
	//Dont add extra arrows on arch_ins
	(void) ss;
}

//arch_unary_arith
void arch_unary_arith::set_target(std::shared_ptr<arch_node> &target){
	this->target=target;
}

std::shared_ptr<arch_node> arch_unary_arith::get_target(){
	return target;
}

//arch_bin_arith
void arch_bin_arith::set_left(std::shared_ptr<arch_node> &left){
	this->left=left;	
}

void arch_bin_arith::set_right(std::shared_ptr<arch_node> &right){
	this->right=right;
}

std::shared_ptr<arch_node> arch_bin_arith::get_left(){
	return left;
}

std::shared_ptr<arch_node> arch_bin_arith::get_right(){
	return right;
}

//arch_comp
logic_op arch_comp::get_logic_op(){
	return op;
}

//arch_num
boost::dynamic_bitset<> arch_num::get_num(){
	return fixed_num;
}

std::string arch_num::get_in_hex(){
    char hex[] = "0123456789ABCDEF";
    std::string num;

    for(unsigned long i=0;i<std::ceil(fixed_num.size()/4.0);i++){
        num=hex[((fixed_num>>(i*4))&boost::dynamic_bitset<>(fixed_num.size(),0xF)).to_ulong()]+num;
    }

	return num;
}

