#include "warning_handler.h"

void warning_handler::def_val_stream_ignored(std::string var_name,int ins){
	std::stringstream emsg;
	emsg<<"warning: "
		<<"on stream "<<var_name<<" "
		<<"default value on instant "
		<<ins
		<<" is ignored in favour of the most recent declaration";

	std::cerr<<emsg.str()<<std::endl;
}

void warning_handler::ins_num_integ_imprecise_representation(std::string var_name,int num,datatype dt){
	std::stringstream emsg;
	emsg<<"warning: "
		<<"on stream "<<var_name<<" "
		<<"imprecise stream instant representation of "
		<<num
		<<" on datatype "
		<<dt.to_string();

	std::cerr<<emsg.str()<<std::endl;
}

void warning_handler::ins_num_frac_imprecise_representation(std::string var_name,double num,datatype dt){
	std::stringstream emsg;
	emsg<<"warning: "
		<<"on stream "<<var_name<<" "
		<<"imprecise stream instant representation of "
		<<num
		<<" on datatype "
		<<dt.to_string();

	std::cerr<<emsg.str()<<std::endl;
}

void warning_handler::num_integ_imprecise_representation(std::string var_name,int num,datatype dt){
	std::stringstream emsg;
	emsg<<"warning: "
		<<"on stream "<<var_name<<" "
		<<"imprecise representation of "
		<<num
		<<" on datatype "
		<<dt.to_string();

	std::cerr<<emsg.str()<<std::endl;
}

void warning_handler::num_frac_imprecise_representation(std::string var_name,double num,datatype dt){
	std::stringstream emsg;
	emsg<<"warning: "
		<<"on stream "<<var_name<<" "
		<<"imprecise representation of "
		<<num
		<<" on datatype "
		<<dt.to_string();

	std::cerr<<emsg.str()<<std::endl;
}
