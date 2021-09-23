#ifndef RTFSS_DEF_WARNING_HANDLER
#define RTFSS_DEF_WARNING_HANDLER

#include <string>
#include <sstream>

#include "../ast/ast.h"


class warning_handler{
	public:
		static void def_val_stream_ignored(std::string var_name,int ins);
		static void num_integ_imprecise_representation(std::string var_name,int num,datatype dt);
		static void num_frac_imprecise_representation(std::string var_name,double num,datatype dt);
		static void ins_num_integ_imprecise_representation(std::string var_name,int num,datatype dt);
		static void ins_num_frac_imprecise_representation(std::string var_name,double num,datatype dt);
};

#endif
