#ifndef RTFSS_DEF_TARGET_GEN_VHDL_TEMPLATES
#define RTFSS_DEF_TARGET_GEN_VHDL_TEMPLATES

#include <boost/dynamic_bitset/dynamic_bitset.hpp>
#include <sstream>
#include <string>
#include <boost/format.hpp>
#include <boost/dynamic_bitset.hpp>

class vhdl_templates{
	private:

		static std::string pulser_template;
		static std::string ffd_template;

		static std::string entity_template;
		static std::string entity_inst_template;
		static std::string entity_generic;
		static std::string entity_port;

		static std::string inst_mapping_template;

		static std::string signal_decl;
		static std::string signal_def_template;
		static std::string signal_assign;
		static std::string signal_resize;

		static std::string add_template;
		static std::string sub_template;
		static std::string sim_template;
		static std::string mult_template;
		static std::string div_template;
		static std::string mod_template;

		static std::string shl_template;
		static std::string shr_template;
		static std::string rtl_template;
		static std::string rtr_template;

		static std::string and_template;
		static std::string or_template;
		static std::string xor_template;
		static std::string not_template;

		static std::string equal_template;
		static std::string diff_template;
		static std::string more_template;
		static std::string less_template;
		static std::string moreq_template;
		static std::string leseq_template;

		static std::string const_template;

	public:
		static void vhdl_pulser(std::stringstream &ss);
		static void vhdl_ffd(std::stringstream &ss);
		
		static void vhdl_entity(std::stringstream &ss,std::string entity_name,std::vector<std::string> generics,std::vector<std::string> ports,std::string arch_name,std::string signals,std::string decls);
		static void vhdl_entity_inst(std::stringstream &ss,std::string inst_name,std::string entity_name,std::vector<std::string> generic_map,std::vector<std::string> port_map);
		static std::string vhdl_entity_generic(std::string gen_name,std::string gen_type);
		static std::string vhdl_entity_port(std::string port_name,std::string inout,unsigned int dim);

		static std::string vhdl_mapping(std::string block_v,std::string our_v);

		static void vhdl_signal(std::stringstream &ss_decl,std::string name,unsigned int dim);
		static void vhdl_assign(std::stringstream &ss_statm,std::string dest,std::string src);
		static void vhdl_resize(std::stringstream &ss_statm,std::string dest,std::string src,std::string type,unsigned int dim);

		static void vhdl_add(std::stringstream &ss_statm,std::string name,std::string left_type,std::string right_type,std::string left_name,std::string right_name);
		static void vhdl_sub(std::stringstream &ss_statm,std::string name,std::string left_type,std::string right_type,std::string left_name,std::string right_name);
		static void vhdl_sim(std::stringstream &ss_statm,std::string name,std::string type,std::string target_name);
		static void vhdl_mult(std::stringstream &ss_statm,std::string name,std::string left_type,std::string right_type,std::string left_name,std::string right_name,unsigned int dim);
		static void vhdl_div(std::stringstream &ss_statm,std::string name,std::string left_type,std::string right_type,std::string left_name,std::string right_name);
		static void vhdl_mod(std::stringstream &ss_statm,std::string name,std::string left_type,std::string right_type,std::string left_name,std::string right_name);

		static void vhdl_shl(std::stringstream &ss_statm,std::string name,std::string type,std::string toshift_name,std::string amnt_name);
		static void vhdl_shr(std::stringstream &ss_statm,std::string name,std::string type,std::string toshift_name,std::string amnt_name);
		static void vhdl_rtl(std::stringstream &ss_statm,std::string name,std::string type,std::string torotate_name,std::string amnt_name);
		static void vhdl_rtr(std::stringstream &ss_statm,std::string name,std::string type,std::string torotate_name,std::string amnt_name);

		static void vhdl_and(std::stringstream &ss_statm,std::string name,std::string left_name,std::string right_name);
		static void vhdl_or(std::stringstream &ss_statm,std::string name,std::string left_name,std::string right_name);
		static void vhdl_xor(std::stringstream &ss_statm,std::string name,std::string left_name,std::string right_name);
		static void vhdl_not(std::stringstream &ss_statm,std::string name,std::string target_name);

		static void vhdl_equal(std::stringstream &ss_statm,std::string name,std::string left_name,std::string right_name);
		static void vhdl_diff(std::stringstream &ss_statm,std::string name,std::string left_name,std::string right_name);
		static void vhdl_more(std::stringstream &ss_statm,std::string name,std::string left_type,std::string left_name,std::string right_type,std::string right_name);
		static void vhdl_less(std::stringstream &ss_statm,std::string name,std::string left_type,std::string left_name,std::string right_type,std::string right_name);
		static void vhdl_moreq(std::stringstream &ss_statm,std::string name,std::string left_type,std::string left_name,std::string right_type,std::string right_name);
		static void vhdl_leseq(std::stringstream &ss_statm,std::string name,std::string left_type,std::string left_name,std::string right_type,std::string right_name);

		static void vhdl_const(std::stringstream &ss_decl,std::string name,unsigned int dim,boost::dynamic_bitset<> value);
};

#endif
