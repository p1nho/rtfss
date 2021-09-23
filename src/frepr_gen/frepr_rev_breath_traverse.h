#ifndef RTFSS_DEF_FREPR_REV_BREATH_TRAVERSE
#define RTFSS_DEF_FREPR_REV_BREATH_TRAVERSE

#include "frepr_node.h"
#include <queue>
#include <memory>
#include <cassert>

class frepr_rev_breath_traverse{

	private:
		frepr_anchor &fanchor;

		void handle_solve(std::shared_ptr<frepr_node> node);
	public:
		virtual void execute();

	protected:
		frepr_rev_breath_traverse(frepr_anchor &anchor):
			fanchor(anchor){}

		virtual void solve_clock(std::shared_ptr<frepr_clock> node)=0;
		virtual void solve_ffd(std::shared_ptr<frepr_ffd> node)=0;
		virtual void solve_pulser(std::shared_ptr<frepr_pulser> node)=0;
		virtual void solve_add(std::shared_ptr<frepr_add> node)=0;
		virtual void solve_sub(std::shared_ptr<frepr_sub> node)=0;
		virtual void solve_sim(std::shared_ptr<frepr_sim> node)=0;
		virtual void solve_mult(std::shared_ptr<frepr_mult> node)=0;
		virtual void solve_div(std::shared_ptr<frepr_div> node)=0;
		virtual void solve_shl(std::shared_ptr<frepr_shl> node)=0;
		virtual void solve_shr(std::shared_ptr<frepr_shr> node)=0;
		virtual void solve_rtl(std::shared_ptr<frepr_rtl> node)=0;
		virtual void solve_rtr(std::shared_ptr<frepr_rtr> node)=0;
		virtual void solve_and(std::shared_ptr<frepr_and> node)=0;
		virtual void solve_or(std::shared_ptr<frepr_or> node)=0;
		virtual void solve_xor(std::shared_ptr<frepr_xor> node)=0;
		virtual void solve_not(std::shared_ptr<frepr_not> node)=0;
		virtual void solve_comp(std::shared_ptr<frepr_comp> node)=0;
		virtual void solve_mod(std::shared_ptr<frepr_mod> node)=0;
		virtual void solve_resize(std::shared_ptr<frepr_resize> node)=0;
		virtual void solve_const(std::shared_ptr<frepr_const> node)=0;

};

#endif
