#include "frepr_gv.h"
#include <boost/dynamic_bitset/dynamic_bitset.hpp>

std::string frepr_gv::gv_label("n");
bool frepr_gv::gv_show_clock(false);

void frepr_gv::execute(){

	ss<<"digraph {"<<std::endl;
	ss<<"graph"<<" [ "
		<<"splines="<<"ortho"<<";"
		<<" ]"<<std::endl;

	frepr_rev_breath_traverse::execute();

	ss<<"}"<<std::endl;

}

std::string frepr_gv::dynbit_to_hex(boost::dynamic_bitset<> db){
    char hex[] = "0123456789ABCDEF";
    std::string num;

    for(unsigned long i=0;i<std::ceil(db.size()/4.0);i++){
        num=hex[((db>>(i*4))&boost::dynamic_bitset<>(db.size(),0xF)).to_ulong()]+num;
    }

    return num;
}

void frepr_gv::draw_unary(std::shared_ptr<frepr_unary_arith> node,std::string op){

	ss<<gv_label<<node->get_node_id()<<" [ "
		<<"label=\""<<"("<<node->get_node_id()<<") "
		<<op<<" "<<node->get_datatype().to_string()<<"\""<<";"
		<<"shape=\""<<"box"<<"\""<<";"
		//<<"; style=bold"<<";"
		<<" ]"<<std::endl;

	//target
	ss<<gv_label<<node->get_target()->get_node_id()<<" -> "
		<<gv_label<<node->get_node_id()<<" [ "
		<<"xlabel="<<"target"<<";"
		<<" ]"<<std::endl;

}

void frepr_gv::draw_bin(std::shared_ptr<frepr_bin_arith> node,std::string op){

	ss<<gv_label<<node->get_node_id()<<" [ "
		<<"label=\""<<"("<<node->get_node_id()<<") "
		<<op<<" "<<node->get_datatype().to_string()<<"\""<<";"
		<<"shape=\""<<"box"<<"\""<<";"
		//<<"; style=bold"<<";"
		<<" ]"<<std::endl;

	//left
	ss<<gv_label<<node->get_left()->get_node_id()<<" -> "
		<<gv_label<<node->get_node_id()<<" [ "
		<<"xlabel="<<"left"<<";"
		<<" ]"<<std::endl;

	//right
	ss<<gv_label<<node->get_right()->get_node_id()<<" -> "
		<<gv_label<<node->get_node_id()<<" [ "
		<<"xlabel="<<"right"<<";"
		<<" ]"<<std::endl;

}

void frepr_gv::solve_clock(std::shared_ptr<frepr_clock> node){
	//std::cerr<<"clock"<<std::endl;

	if(frepr_gv::gv_show_clock){
		ss<<gv_label<<node->get_node_id()<<" [ "
			<<"label=\""<<"clock"<<"\""<<";"
			<<"shape=\""<<"box"<<"\""<<";"
			//<<"; style=bold"
			<<" ]"<<std::endl;
	}
}

void frepr_gv::solve_ffd(std::shared_ptr<frepr_ffd> node){
	//std::cerr<<"ffd"<<std::endl;
	std::string def(dynbit_to_hex(node->get_default_val()));
	if(!def.empty()){
		def=" def:"+def;
	}

	ss<<gv_label<<node->get_node_id()<<" [ "
		<<"label=\""<<"("<<node->get_node_id()<<") "
		<<"ffd "<<node->get_name()<<" "<<node->get_datatype().to_string()
		<<def<<"\""<<";"
		<<"shape=\""<<"box"<<"\""<<";"
		//<<"; style=bold"<<";"
		<<" ]"<<std::endl;

	//src
	if(node->get_src()!=nullptr){ //When ffd is var in from a cblock, it has no source
		ss<<gv_label<<node->get_src()->get_node_id()<<" -> "
			<<gv_label<<node->get_node_id()<<" [ "
			//<<"xlabel="<<"src"<<";"
			<<"color="<<"black"<<";"
			<<" ]"<<std::endl;
	}

	//clk
	if(frepr_gv::gv_show_clock){
		ss<<gv_label<<node->get_clk()->get_node_id()<<" -> "
			<<gv_label<<node->get_node_id()<<" [ "
			//<<"xlabel="<<"clk"<<";"
			<<"color="<<"red"<<";"
			<<" ]"<<std::endl;
	}

	//en
	if(node->get_en()!=nullptr){
		ss<<gv_label<<node->get_en()->get_node_id()<<" -> "
			<<gv_label<<node->get_node_id()<<" [ "
			//<<"xlabel="<<"en"<<";"
			<<"color="<<"blue"<<";"
			<<" ]"<<std::endl;
	}
	
}

void frepr_gv::solve_pulser(std::shared_ptr<frepr_pulser> node){
	//std::cerr<<"pulser"<<std::endl;

	ss<<gv_label<<node->get_node_id()<<" [ "
		<<"label=\""<<"("<<node->get_node_id()<<") "
		<<"pulser "<<node->get_val()<<node->get_ts().to_string()<<"\""<<";"
		<<"shape=\""<<"box"<<"\""<<";"
		//<<"; style=bold"<<";"
		<<" ]"<<std::endl;
}

void frepr_gv::solve_add(std::shared_ptr<frepr_add> node){
	//std::cerr<<"add"<<std::endl;
	draw_bin(node,"+");
}

void frepr_gv::solve_sub(std::shared_ptr<frepr_sub> node){
	//std::cerr<<"sub"<<std::endl;
	draw_bin(node,"-");
}

void frepr_gv::solve_sim(std::shared_ptr<frepr_sim> node){
	//std::cerr<<"sim"<<std::endl;
	draw_unary(node,"-");
}

void frepr_gv::solve_mult(std::shared_ptr<frepr_mult> node){
	//std::cerr<<"mult"<<std::endl;
	draw_bin(node,"*");
}

void frepr_gv::solve_div(std::shared_ptr<frepr_div> node){
	//std::cerr<<"div"<<std::endl;
	draw_bin(node,"/");
}

void frepr_gv::solve_shl(std::shared_ptr<frepr_shl> node){
	//std::cerr<<"shl"<<std::endl;
	draw_bin(node,"<<");
}

void frepr_gv::solve_shr(std::shared_ptr<frepr_shr> node){
	//std::cerr<<"shr"<<std::endl;
	draw_bin(node,">>");
}

void frepr_gv::solve_rtl(std::shared_ptr<frepr_rtl> node){
	//std::cerr<<"rtl"<<std::endl;
	draw_bin(node,"<<<");
}

void frepr_gv::solve_rtr(std::shared_ptr<frepr_rtr> node){
	//std::cerr<<"rtr"<<std::endl;
	draw_bin(node,">>>");
}

void frepr_gv::solve_and(std::shared_ptr<frepr_and> node){
	//std::cerr<<"and"<<std::endl;
	draw_bin(node,"and");
}

void frepr_gv::solve_or(std::shared_ptr<frepr_or> node){
	//std::cerr<<"or"<<std::endl;
	draw_bin(node,"or");
}

void frepr_gv::solve_xor(std::shared_ptr<frepr_xor> node){
	//std::cerr<<"xor"<<std::endl;
	draw_bin(node,"xor");
}

void frepr_gv::solve_not(std::shared_ptr<frepr_not> node){
	//std::cerr<<"rtr"<<std::endl;
	draw_unary(node,"not");
}

void frepr_gv::solve_comp(std::shared_ptr<frepr_comp> node){
	//std::cerr<<"comp"<<std::endl;
	draw_bin(node,node->get_logic_op().to_string());
}

void frepr_gv::solve_mod(std::shared_ptr<frepr_mod> node){
	//std::cerr<<"mod"<<std::endl;
	draw_bin(node,"%");
}

void frepr_gv::solve_resize(std::shared_ptr<frepr_resize> node){
	//std::cerr<<"resize"<<std::endl;

	ss<<gv_label<<node->get_node_id()<<" [ "
		<<"label=\""<<"("<<node->get_node_id()<<") "
		<<"resize"<<" "<<node->get_datatype().to_string()<<"\""<<";"
		<<"shape=\""<<"box"<<"\""<<";"
		//<<"; style=bold"<<";"
		<<" ]"<<std::endl;

	//target
	ss<<gv_label<<node->get_target()->get_node_id()<<" -> "
		<<gv_label<<node->get_node_id()<<" [ "
		<<"xlabel="<<"target"<<";"
		<<" ]"<<std::endl;

}

void frepr_gv::solve_const(std::shared_ptr<frepr_const> node){
	//std::cerr<<"const"<<std::endl;
	std::string num(dynbit_to_hex(node->get_const()));
	ss<<gv_label<<node->get_node_id()<<" [ "
		<<"label=\""<<"("<<node->get_node_id()<<") "
		<<"const "<<num<<" "<<node->get_datatype().to_string()<<"\""<<";"
		<<"shape=\""<<"box"<<"\""<<";"
		//<<"; style=bold"<<";"
		<<" ]"<<std::endl;

}

