#ifndef RTFSS_DEF_ERROR_HANDLER
#define RTFSS_DEF_ERROR_HANDLER

#include <string>
#include <sstream>

#include "../ast/ast.h"
#include "../var_solver/var.h"

class error_handler{
	public:
		static void syntax_error(); 
		static void generic_not_implemented(); 

		static void pulse_no_decl(std::string var_name); 
		static void stream_no_decl(std::string var_name); 
		static void pulse_redecl(std::string var_name); 
		static void stream_redecl(std::string var_name); 

		static void stream_dep_loop(std::shared_ptr<var_complete_scope> vcscope,std::vector<unsigned int> &var_seq); 

		static void const_stream_ins(std::string var_name);

		static void assign_in_stream(std::string var_name);

		static void datatype_mismatch(std::string var_name,datatype expected,datatype got); 
		static void type_mismatch(std::string var_name,datatype expected,datatype got); 
		static void size_mismatch(std::string var_name,datatype expected,datatype got); 
		static void type_size_mismatch(std::string var_name,datatype expected,datatype got); 

		static void const_mod_not_int(double val);

		static void inst_not_const(std::string name);
		static void inst_attr_not_const(std::string name,int ins);
		static void inst_not_int(std::string name,double val);

		static void mod_frac_right(std::string var_name);
		static void mod_frac_left(std::string var_name);

		static void shl_right_not_const(std::string var_name);
		static void shr_right_not_const(std::string var_name);
		static void shl_frac_shift(std::string var_name);
		static void shr_frac_shift(std::string var_name);
		static void shl_not_unsigned(std::string var_name);
		static void shr_not_unsigned(std::string var_name);
		static void rtl_right_not_const(std::string var_name);
		static void rtr_right_not_const(std::string var_name);
		static void rtl_frac_rotate(std::string var_name);
		static void rtr_frac_rotate(std::string var_name);
		static void rtl_right_not_unsigned(std::string var_name);
		static void rtr_right_not_unsigned(std::string var_name);
		static void rtl_left_not_unsigned(std::string var_name);
		static void rtr_left_not_unsigned(std::string var_name);

		static void cblock_not_main(std::string cblock_name);
};

class rtfss_error{
};

#endif
