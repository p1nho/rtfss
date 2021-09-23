#include "arch_node.h"

std::string arch_add::to_string(){
	return "+ ("+std::to_string(get_node_id())
		//+","+std::to_string(get_pulse_id())
		+","+get_datatype().to_string()+")";
}

std::string arch_sub::to_string(){
	return "- ("+std::to_string(get_node_id())
		//+","+std::to_string(get_pulse_id())
		+","+get_datatype().to_string()+")";
}

std::string arch_sim::to_string(){
	return "- ("+std::to_string(get_node_id())
		//+","+std::to_string(get_pulse_id())
		+","+get_datatype().to_string()+")";
}

std::string arch_mult::to_string(){
	return "* ("+std::to_string(get_node_id())
		//+","+std::to_string(get_pulse_id())
		+","+get_datatype().to_string()+")";
}

std::string arch_div::to_string(){
	return "/ ("+std::to_string(get_node_id())
		//+","+std::to_string(get_pulse_id())
		+","+get_datatype().to_string()+")";
}

std::string arch_shl::to_string(){
	return "<< ("+std::to_string(get_node_id())
		//+","+std::to_string(get_pulse_id())
		+","+get_datatype().to_string()+")";
}

std::string arch_shr::to_string(){
	return ">> ("+std::to_string(get_node_id())
		//+","+std::to_string(get_pulse_id())
		+","+get_datatype().to_string()+")";
}

std::string arch_rtl::to_string(){
	return "<<< ("+std::to_string(get_node_id())
		//+","+std::to_string(get_pulse_id())
		+","+get_datatype().to_string()+")";
}

std::string arch_rtr::to_string(){
	return ">>> ("+std::to_string(get_node_id())
		//+","+std::to_string(get_pulse_id())
		+","+get_datatype().to_string()+")";
}

std::string arch_and::to_string(){
	return "and ("+std::to_string(get_node_id())
		//+","+std::to_string(get_pulse_id())
		+","+get_datatype().to_string()+")";
}

std::string arch_or::to_string(){
	return "or ("+std::to_string(get_node_id())
		//+","+std::to_string(get_pulse_id())
		+","+get_datatype().to_string()+")";
}

std::string arch_xor::to_string(){
	return "xor ("+std::to_string(get_node_id())
		//+","+std::to_string(get_pulse_id())
		+","+get_datatype().to_string()+")";
}

std::string arch_not::to_string(){
	return "not ("+std::to_string(get_node_id())
		//+","+std::to_string(get_pulse_id())
		+","+get_datatype().to_string()+")";
}

/*std::string arch_gap::to_string(){
	return "gap ("+std::to_string(get_node_id())
		//+","+std::to_string(get_pulse_id())
		+","+get_datatype().to_string()+")";
}*/

std::string arch_comp::to_string(){
	return op.to_string()+" ("+std::to_string(get_node_id())
		//+","+std::to_string(get_pulse_id())
		+","+get_datatype().to_string()+")";
}

/*std::string arch_prop::to_string(){
	return "& ("+std::to_string(get_node_id())
		+","+std::to_string(get_pulse_id())
		+","+get_datatype().to_string()+")";
}*/

std::string arch_mod::to_string(){
	return "% ("+std::to_string(get_node_id())
		//+","+std::to_string(get_pulse_id())
		+","+get_datatype().to_string()+")";
}

std::string arch_num::to_string(){
	return get_in_hex()+" ("+std::to_string(get_node_id())
		//+","+std::to_string(get_pulse_id())
		+","+get_datatype().to_string()+")";
}
 
std::string arch_cast::to_string(){
	return "<cast> ("+std::to_string(get_node_id())
		//+","+std::to_string(get_pulse_id())
		+","+get_datatype().to_string()+")";
}

std::string arch_ins::to_string(){
	std::string def_val;
	if(!default_value.empty()){
		def_val=" def: "+get_df_in_hex();
	}
	return name+"'"+std::to_string(-int(inst)-1)
		+def_val
		+" ("+std::to_string(get_node_id())
		//+","+std::to_string(get_pulse_id())
		+","+get_datatype().to_string()+")";
}

std::string arch_id::to_string(){
	return name+" ("+std::to_string(get_node_id())
		//+","+std::to_string(get_pulse_id())
		+","+get_datatype().to_string()+")";
}
 
