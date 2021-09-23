#include "error_handler.h"

void error_handler::syntax_error(){
	std::stringstream emsg;
	emsg<<"error: "
		<<"syntax error";

	std::cerr<<emsg.str()<<std::endl;
	throw rtfss_error();
}

void error_handler::generic_not_implemented(){
	std::stringstream emsg;
	emsg<<"error: "
		<<"Unimplemented feature used";

	std::cerr<<emsg.str()<<std::endl;
	throw rtfss_error();
}

void error_handler::pulse_no_decl(std::string var_name){
	std::stringstream emsg;
	emsg<<"error: "
		<<"pulse "<<var_name<<" "
		<<"used but not declared";

	std::cerr<<emsg.str()<<std::endl;
	throw rtfss_error();
}

void error_handler::stream_no_decl(std::string var_name){
	std::stringstream emsg;
	emsg<<"error: "
		<<"stream "<<var_name<<" "
		<<"used but not declared";

	std::cerr<<emsg.str()<<std::endl;
	throw rtfss_error();
}

void error_handler::pulse_redecl(std::string var_name){
	std::stringstream emsg;
	emsg<<"error: "
		<<"pulse "<<var_name<<" "
		<<"already declared";

	std::cerr<<emsg.str()<<std::endl;
	throw rtfss_error();
}

void error_handler::stream_redecl(std::string var_name){
	std::stringstream emsg;
	emsg<<"error: "
		<<"stream "<<var_name<<" "
		<<"already declared";

	std::cerr<<emsg.str()<<std::endl;
	throw rtfss_error();
}


void error_handler::stream_dep_loop(std::shared_ptr<var_complete_scope> vcscope,std::vector<unsigned int> &var_seq){
	std::vector<std::shared_ptr<var_property>> &vp(vcscope->get());

	std::stringstream emsg;
	emsg<<"error: "
		<<"stream dependency loop on ";

	for(auto var=var_seq.begin();var!=var_seq.end()-1;var++){
		emsg<<vp[*var]->name<<" -> ";
	}
	emsg<<vp[var_seq[0]]->name;

	std::cerr<<emsg.str()<<std::endl;
	throw rtfss_error();
}

void error_handler::datatype_mismatch(std::string var_name,datatype expected,datatype got){
	bool eq_type(expected.get_datatype()==got.get_datatype());
	bool eq_size(expected.get_frac()==got.get_frac()&&expected.get_integ()==got.get_integ());

	if(!eq_type&&eq_size){
		type_mismatch(var_name,expected,got);
	}
	else if(eq_type&&!eq_size){
		size_mismatch(var_name,expected,got);
	}
	else{
		type_size_mismatch(var_name,expected,got);
	}
}

void error_handler::const_stream_ins(std::string var_name){
	std::stringstream emsg;
	emsg<<"error: "
		<<"on stream "<<var_name<<" "
		<<"instant operator not allowed in const stream";

	std::cerr<<emsg.str()<<std::endl;
	throw rtfss_error();
}

void error_handler::assign_in_stream(std::string var_name){
	std::stringstream emsg;
	emsg<<"error: "
		<<"on stream "<<var_name<<" "
		<<"can't assign to CBlock input stream";

	std::cerr<<emsg.str()<<std::endl;
	throw rtfss_error();
}

void error_handler::type_mismatch(std::string var_name,datatype expected,datatype got){
	std::stringstream emsg;
	emsg<<"error: "
		<<"on stream "<<var_name<<" "
		<<"type mismatch "
		<<"expected: "<<expected.to_string()<<" "
		<<"got: "<<got.to_string();

	std::cerr<<emsg.str()<<std::endl;
	throw rtfss_error();
}

void error_handler::size_mismatch(std::string var_name,datatype expected,datatype got){
	std::stringstream emsg;
	emsg<<"error: "
		<<"on stream "<<var_name<<" "
		<<"size mismatch "
		<<"expected: "<<expected.to_string()<<" "
		<<"got: "<<got.to_string();

	std::cerr<<emsg.str()<<std::endl;
	throw rtfss_error();
}

void error_handler::type_size_mismatch(std::string var_name,datatype expected,datatype got){
	std::stringstream emsg;
	emsg<<"error: "
		<<"on stream "<<var_name<<" "
		<<"type and size mismatch "
		<<"expected: "<<expected.to_string()<<" "
		<<"got: "<<got.to_string();

	std::cerr<<emsg.str()<<std::endl;
	throw rtfss_error();
}

void error_handler::const_mod_not_int(double val){
	std::stringstream emsg;
	emsg<<"error: "
		<<"modulus operator requires operands to be integer "
		<<"("<<val<<" is not integer)";

	std::cerr<<emsg.str()<<std::endl;
	throw rtfss_error();
}

void error_handler::inst_not_int(std::string name,double val){
	std::stringstream emsg;
	emsg<<"error: "
		<<"instant indexing on stream "
		<<name<<" requires an integer "
		<<"("<<val<<" is not integer)";

	std::cerr<<emsg.str()<<std::endl;
	throw rtfss_error();
}

void error_handler::inst_attr_not_const(std::string name,int ins){
	std::stringstream emsg;
	emsg<<"error: "
		<<"default value of instant index "
	   	<<ins<<" of stream "
		<<name<<" can't be solved to a constant; "
		<<"as to be numeric or cast to numeric)";

	std::cerr<<emsg.str()<<std::endl;
	throw rtfss_error();
}

void error_handler::inst_not_const(std::string name){
	std::stringstream emsg;
	emsg<<"error: "
		<<"instant indexing of stream "
		<<name<<" can't be solved to a constant";

	std::cerr<<emsg.str()<<std::endl;
	throw rtfss_error();
}

void error_handler::mod_frac_right(std::string var_name){
	std::stringstream emsg;
	emsg<<"error: "
		<<"on stream "<<var_name<<" "
		<<"right operator of modulus operation is fractional (has to be int)";

	std::cerr<<emsg.str()<<std::endl;
	throw rtfss_error();
}

void error_handler::mod_frac_left(std::string var_name){
	std::stringstream emsg;
	emsg<<"error: "
		<<"on stream "<<var_name<<" "
		<<"left operator of modulus operation is fractional (has to be int)";

	std::cerr<<emsg.str()<<std::endl;
	throw rtfss_error();
}
void error_handler::shr_right_not_const(std::string var_name){
	std::stringstream emsg;
	emsg<<"error: "
		<<"on stream "<<var_name<<" "
		<<"shift right operation right operator is not const";

	std::cerr<<emsg.str()<<std::endl;
	throw rtfss_error();
}

void error_handler::shl_right_not_const(std::string var_name){
	std::stringstream emsg;
	emsg<<"error: "
		<<"on stream "<<var_name<<" "
		<<"shift left operation right operator is not const";

	std::cerr<<emsg.str()<<std::endl;
	throw rtfss_error();
}

void error_handler::shr_frac_shift(std::string var_name){
	std::stringstream emsg;
	emsg<<"error: "
		<<"on stream "<<var_name<<" "
		<<"can't shift right a fractional amount of bits";

	std::cerr<<emsg.str()<<std::endl;
	throw rtfss_error();
}

void error_handler::shl_frac_shift(std::string var_name){
	std::stringstream emsg;
	emsg<<"error: "
		<<"on stream "<<var_name<<" "
		<<"can't shift left a fractional amount of bits";

	std::cerr<<emsg.str()<<std::endl;
	throw rtfss_error();
}

void error_handler::shr_not_unsigned(std::string var_name){
	std::stringstream emsg;
	emsg<<"error: "
		<<"on stream "<<var_name<<" "
		<<"shift right amount is not unsigned";

	std::cerr<<emsg.str()<<std::endl;
	throw rtfss_error();
}

void error_handler::shl_not_unsigned(std::string var_name){
	std::stringstream emsg;
	emsg<<"error: "
		<<"on stream "<<var_name<<" "
		<<"shift left amount is not unsigned";

	std::cerr<<emsg.str()<<std::endl;
	throw rtfss_error();
}

void error_handler::rtr_right_not_const(std::string var_name){
	std::stringstream emsg;
	emsg<<"error: "
		<<"on stream "<<var_name<<" "
		<<"rotate right operation right operator is not const";

	std::cerr<<emsg.str()<<std::endl;
	throw rtfss_error();
}

void error_handler::rtl_right_not_const(std::string var_name){
	std::stringstream emsg;
	emsg<<"error: "
		<<"on stream "<<var_name<<" "
		<<"rotate left operation right operator is not const";

	std::cerr<<emsg.str()<<std::endl;
	throw rtfss_error();
}

void error_handler::rtr_frac_rotate(std::string var_name){
	std::stringstream emsg;
	emsg<<"error: "
		<<"on stream "<<var_name<<" "
		<<"can't rotate right a fractional amount of bits";

	std::cerr<<emsg.str()<<std::endl;
	throw rtfss_error();
}

void error_handler::rtl_frac_rotate(std::string var_name){
	std::stringstream emsg;
	emsg<<"error: "
		<<"on stream "<<var_name<<" "
		<<"can't rotate left a fractional amount of bits";

	std::cerr<<emsg.str()<<std::endl;
	throw rtfss_error();
}

void error_handler::rtr_right_not_unsigned(std::string var_name){
	std::stringstream emsg;
	emsg<<"error: "
		<<"on stream "<<var_name<<" "
		<<"rotate right amount is not unsigned";

	std::cerr<<emsg.str()<<std::endl;
	throw rtfss_error();
}

void error_handler::rtl_right_not_unsigned(std::string var_name){
	std::stringstream emsg;
	emsg<<"error: "
		<<"on stream "<<var_name<<" "
		<<"rotate left amount is not unsigned";

	std::cerr<<emsg.str()<<std::endl;
	throw rtfss_error();
}

void error_handler::rtr_left_not_unsigned(std::string var_name){
	std::stringstream emsg;
	emsg<<"error: "
		<<"on stream "<<var_name<<" "
		<<"stream to be rotated right is not unsigned";

	std::cerr<<emsg.str()<<std::endl;
	throw rtfss_error();
}

void error_handler::rtl_left_not_unsigned(std::string var_name){
	std::stringstream emsg;
	emsg<<"error: "
		<<"on stream "<<var_name<<" "
		<<"stream to be rotated left is not unsigned";

	std::cerr<<emsg.str()<<std::endl;
	throw rtfss_error();
}

void error_handler::cblock_not_main(std::string cblock_name){
	std::stringstream emsg;
	emsg<<"error: "
		<<"cblock "<<cblock_name<<" "
		<<"is not main (only main cblock is allowed on this compiler)";

	std::cerr<<emsg.str()<<std::endl;
	throw rtfss_error();
}
