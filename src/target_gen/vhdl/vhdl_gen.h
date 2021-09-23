#ifndef RTFSS_DEF_TARGET_GEN_VHDL_GEN
#define RTFSS_DEF_TARGET_GEN_VHDL_GEN

#include "../../frepr_gen/frepr_node.h"
#include "../../frepr_gen/frepr_rev_breath_traverse.h"
#include "vhdl_templates.h"
#include <boost/dynamic_bitset/dynamic_bitset.hpp>
#include <queue>
#include <memory>
#include <cassert>
#include <sstream>
#include <string>

class vhdl_gen : public frepr_rev_breath_traverse{

	public:
		vhdl_gen(frepr_anchor &anchor,std::stringstream &ss):
			frepr_rev_breath_traverse(anchor),
			ss_out(ss),
			inst_cnt(0){}

		virtual void execute();

	private:
		static std::string vprefix;
		static std::string pprefix;
		static std::string master_clock;
		static std::string master_reset;

		std::stringstream &ss_out;
		std::stringstream ss_var_dcl;
		std::stringstream ss_statm;
		std::vector<std::string> vs_generics;
		std::vector<std::string> vs_ports;

		unsigned int clock_id;
		unsigned int inst_cnt;

		std::string solve_type(datatype dt);

		void spawn_ffd(std::string name,int dim,std::string en,boost::dynamic_bitset<> rst_val,std::string in);

		virtual void solve_clock(std::shared_ptr<frepr_clock> node);
		virtual void solve_ffd(std::shared_ptr<frepr_ffd> node);
		virtual void solve_pulser(std::shared_ptr<frepr_pulser> node);
		virtual void solve_add(std::shared_ptr<frepr_add> node);
		virtual void solve_sub(std::shared_ptr<frepr_sub> node);
		virtual void solve_sim(std::shared_ptr<frepr_sim> node);
		virtual void solve_mult(std::shared_ptr<frepr_mult> node);
		virtual void solve_div(std::shared_ptr<frepr_div> node);
		virtual void solve_shl(std::shared_ptr<frepr_shl> node);
		virtual void solve_shr(std::shared_ptr<frepr_shr> node);
		virtual void solve_rtl(std::shared_ptr<frepr_rtl> node);
		virtual void solve_rtr(std::shared_ptr<frepr_rtr> node);
		virtual void solve_and(std::shared_ptr<frepr_and> node);
		virtual void solve_or(std::shared_ptr<frepr_or> node);
		virtual void solve_xor(std::shared_ptr<frepr_xor> node);
		virtual void solve_not(std::shared_ptr<frepr_not> node);
		virtual void solve_comp(std::shared_ptr<frepr_comp> node);
		virtual void solve_mod(std::shared_ptr<frepr_mod> node);
		virtual void solve_resize(std::shared_ptr<frepr_resize> node);
		virtual void solve_const(std::shared_ptr<frepr_const> node);

};

#endif
