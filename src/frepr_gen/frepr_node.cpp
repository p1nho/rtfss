#include "frepr_node.h"

//frepr_node
frepr_node::frepr_op frepr_node::get_op(){
	return op;
}

unsigned int frepr_node::get_node_id(){
	return frepr_id;
}

//frepr_anchor
void frepr_anchor::add_end_node(std::shared_ptr<frepr_node> node){
	end_nodes.push_back(node);
}

std::vector<std::shared_ptr<frepr_node>>& frepr_anchor::get_nodes(){
	return end_nodes;	
}

std::vector<std::shared_ptr<frepr_node>> frepr_anchor::get_ins(){
	return end_nodes;
}

//frepr_clock
std::vector<std::shared_ptr<frepr_node>> frepr_clock::get_ins(){
	return std::vector<std::shared_ptr<frepr_node>>();
}

//frepr_arith
datatype frepr_arith::get_datatype(){
	return dt;
}

//frepr_ffd
void frepr_ffd::set_src(std::shared_ptr<frepr_node> src){
	this->src=src;	
}

void frepr_ffd::set_clk(std::shared_ptr<frepr_node> clk){
	this->clk=clk;
}

void frepr_ffd::set_en(std::shared_ptr<frepr_node> en){
	this->en=en;
}

void frepr_ffd::set_vk(var_property::var_kind vk){
	this->vk=vk;
}

void frepr_ffd::set_default_val(boost::dynamic_bitset<> dv){
	default_val=dv;
}

void frepr_ffd::set_name(std::string name){
	this->name=name;
}

void frepr_ffd::set_cblock_name(std::string cblk_name){
	cblock_name=cblk_name;
}

std::shared_ptr<frepr_node> frepr_ffd::get_src(){
	return src;
}

std::shared_ptr<frepr_node> frepr_ffd::get_clk(){
	return clk;
}

std::shared_ptr<frepr_node> frepr_ffd::get_en(){
	return en;
}

boost::dynamic_bitset<> frepr_ffd::get_default_val(){
	return default_val;
}

std::string frepr_ffd::get_cblock_name(){
	return cblock_name;
}

std::string frepr_ffd::get_name(){
	return name;
}

var_property::var_kind frepr_ffd::get_vk(){
	return vk;
}

std::vector<std::shared_ptr<frepr_node>> frepr_ffd::get_ins(){
	if(en==nullptr){ //always enabled
		return {src,clk};
	}
	else if(src==nullptr){ //in var on a cblock
		return {clk,en};
	}
	else{
		return {src,clk,en};
	}
}

//frepr_pulser
void frepr_pulser::set_ts(time_scale ts){
	this->ts=ts;	
}

void frepr_pulser::set_val(double val){
	this->val=val;
}

time_scale frepr_pulser::get_ts(){
	return ts;
}

double frepr_pulser::get_val(){
	return val;
}

std::vector<std::shared_ptr<frepr_node>> frepr_pulser::get_ins(){
	return std::vector<std::shared_ptr<frepr_node>>();
}

//frepr_unary_arith
void frepr_unary_arith::set_target(std::shared_ptr<frepr_arith> &target){
	this->target=target;
}

std::shared_ptr<frepr_arith> frepr_unary_arith::get_target(){
	return target;
}

std::vector<std::shared_ptr<frepr_node>> frepr_unary_arith::get_ins(){
	return {target};
}

//frepr_bin_arith
void frepr_bin_arith::set_left(std::shared_ptr<frepr_arith> &left){
	this->left=left;
}

void frepr_bin_arith::set_right(std::shared_ptr<frepr_arith> &right){
	this->right=right;
}

std::shared_ptr<frepr_arith> frepr_bin_arith::get_left(){
	return left;
}

std::shared_ptr<frepr_arith> frepr_bin_arith::get_right(){
	return right;
}

std::vector<std::shared_ptr<frepr_node>> frepr_bin_arith::get_ins(){
	return {left,right};
}

//frepr_comp
logic_op frepr_comp::get_logic_op(){
	return op;
}

//frepr_const
boost::dynamic_bitset<> frepr_const::get_const(){
	return fixed_num;
}

std::vector<std::shared_ptr<frepr_node>> frepr_const::get_ins(){
	return std::vector<std::shared_ptr<frepr_node>>();
}
