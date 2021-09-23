#ifndef RTFSS_DEF_ARCH_TOPL_TRAVESE
#define RTFSS_DEF_ARCH_TOPL_TRAVESE

#include "arch_graph.h"

#include <deque>

class arch_topl_traverse{

	private:
		std::map<std::shared_ptr<arch_node>,std::vector<std::shared_ptr<arch_node>>,std::owner_less<std::shared_ptr<arch_node>>> &inv_adj_list;
		std::map<std::shared_ptr<arch_node>,std::vector<std::shared_ptr<arch_node>>,std::owner_less<std::shared_ptr<arch_node>>> &adj_list;
		std::map<unsigned int,std::shared_ptr<arch_node>> &nodes;

		void handle_solve(std::shared_ptr<arch_node> node);
	public:
		virtual void execute();

	protected:
		arch_topl_traverse(arch_graph &graph):
			inv_adj_list(graph.get_inv_adj_list()),
			adj_list(graph.get_adj_list()),
			nodes(graph.get_nodes()){}

		virtual void solve_add(std::shared_ptr<arch_add> node)=0;
		virtual void solve_sub(std::shared_ptr<arch_sub> node)=0;
		virtual void solve_sim(std::shared_ptr<arch_sim> node)=0;
		virtual void solve_mult(std::shared_ptr<arch_mult> node)=0;
		virtual void solve_div(std::shared_ptr<arch_div> node)=0;
		virtual void solve_shl(std::shared_ptr<arch_shl> node)=0;
		virtual void solve_shr(std::shared_ptr<arch_shr> node)=0;
		virtual void solve_rtl(std::shared_ptr<arch_rtl> node)=0;
		virtual void solve_rtr(std::shared_ptr<arch_rtr> node)=0;
		virtual void solve_and(std::shared_ptr<arch_and> node)=0;
		virtual void solve_or(std::shared_ptr<arch_or> node)=0;
		virtual void solve_xor(std::shared_ptr<arch_xor> node)=0;
		virtual void solve_not(std::shared_ptr<arch_not> node)=0;
		virtual void solve_comp(std::shared_ptr<arch_comp> node)=0;
		virtual void solve_mod(std::shared_ptr<arch_mod> node)=0;
		virtual void solve_cast(std::shared_ptr<arch_cast> node)=0;
		virtual void solve_num(std::shared_ptr<arch_num> node)=0;
		virtual void solve_id(std::shared_ptr<arch_id> node)=0;
		virtual void solve_ins(std::shared_ptr<arch_ins> node)=0;

		//virtual void solve_gap(std::shared_ptr<ast_gap> node)=0;
		//virtual void solve_prop(std::shared_ptr<ast_prop> node)=0;
		//virtual void solve_valins(std::shared_ptr<ast_valins> node)=0;

};

#endif
