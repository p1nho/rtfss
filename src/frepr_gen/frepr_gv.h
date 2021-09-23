#ifndef RTFSS_DEF_FREPR_GV
#define RTFSS_DEF_FREPR_GV

#include "frepr_rev_breath_traverse.h"
#include <boost/dynamic_bitset/dynamic_bitset.hpp>
#include <queue>
#include <cmath>
#include <sstream>
#include <boost/dynamic_bitset.hpp>

class frepr_gv : public frepr_rev_breath_traverse{


	public:
		virtual void execute();

		frepr_gv(frepr_anchor &anchor,std::stringstream &ss):
			frepr_rev_breath_traverse(anchor),
			ss(ss){}

	private:
		static std::string gv_label; 
		static bool gv_show_clock; 

		std::stringstream &ss;

		std::string dynbit_to_hex(boost::dynamic_bitset<> db);

		void draw_unary(std::shared_ptr<frepr_unary_arith> node,std::string op);
		void draw_bin(std::shared_ptr<frepr_bin_arith> node,std::string op);

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
