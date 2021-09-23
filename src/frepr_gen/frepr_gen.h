#ifndef RTFSS_DEF_FINAL_REPR_GEN
#define RTFSS_DEF_FINAL_REPR_GEN

#include "../ast/ast.h"
#include "../ast/ast_property.h"
#include "../ast/ast_visitor.h"
#include "../var_solver/var.h"
#include "../arch_gen/arch_graph.h"
#include "../arch_gen/arch_node.h"
#include "../arch_gen/arch_topl_traverse.h"
#include "../arch_gen/arch_property.h"
#include "frepr_node.h"

#include <cassert>
#include <memory>
#include <vector>
#include <set>
#include <iostream>
#include <memory>
#include <boost/dynamic_bitset.hpp>

//Final representation generator, last step before target code
//Creates the hardware block graphs from the architectural graph
//Here the pipeline is designed
class frepr_gen : public arch_topl_traverse{

	private:
		arch_graph& graph;
		frepr_anchor& fanchor;
		std::shared_ptr<pulse_complete_scope> pcscope;

		unsigned int node_cnt;
		std::shared_ptr<frepr_clock> mclock; //Master clock node
		std::vector<std::vector<std::shared_ptr<frepr_ffd>>> pulse_chain;
		std::vector<std::shared_ptr<frepr_pulser>> pulse_nodes;
		arch_property<std::pair<unsigned int,unsigned int>> curr_pulse;
		arch_property<std::shared_ptr<frepr_node>> gen_frepr;
		std::map<std::shared_ptr<frepr_ffd>,std::shared_ptr<arch_node>> ins_map;

		void create_pulse_nodes();

		std::shared_ptr<frepr_ffd> get_pulse(std::pair<unsigned int,unsigned int> *past_p,unsigned int pid);
		std::shared_ptr<frepr_ffd> start_chain(unsigned int pid);
		std::pair<std::shared_ptr<frepr_arith>,std::shared_ptr<frepr_arith>> resolve_bin(std::shared_ptr<arch_node> right,std::shared_ptr<arch_node> left,unsigned int pid,std::pair<std::shared_ptr<frepr_ffd>,std::pair<unsigned int,unsigned int>> &p_to_use);

		std::shared_ptr<frepr_arith> align_stream(std::shared_ptr<frepr_arith> in,datatype& target);
		datatype inclusive_datatype(datatype::dt_types ddt,datatype dt0,datatype dt1);

		void solve_add(std::shared_ptr<arch_add> node);
		void solve_sub(std::shared_ptr<arch_sub> node);
		void solve_sim(std::shared_ptr<arch_sim> node);
		void solve_mult(std::shared_ptr<arch_mult> node);
		void solve_div(std::shared_ptr<arch_div> node);
		void solve_shl(std::shared_ptr<arch_shl> node);
		void solve_shr(std::shared_ptr<arch_shr> node);
		void solve_rtl(std::shared_ptr<arch_rtl> node);
		void solve_rtr(std::shared_ptr<arch_rtr> node);
		void solve_and(std::shared_ptr<arch_and> node);
		void solve_or(std::shared_ptr<arch_or> node);
		void solve_xor(std::shared_ptr<arch_xor> node);
		void solve_not(std::shared_ptr<arch_not> node);
		void solve_comp(std::shared_ptr<arch_comp> node);
		void solve_mod(std::shared_ptr<arch_mod> node);
		void solve_cast(std::shared_ptr<arch_cast> node);
		void solve_num(std::shared_ptr<arch_num> node);
		void solve_id(std::shared_ptr<arch_id> node);
		void solve_ins(std::shared_ptr<arch_ins> node);

	public:
		frepr_gen(arch_graph& graph,frepr_anchor& fanchor,std::shared_ptr<pulse_complete_scope> pcscope):
			arch_topl_traverse(graph),
			graph(graph),
			fanchor(fanchor),
			pcscope(pcscope),
			pulse_chain(pcscope->get().size()),
			pulse_nodes(pcscope->get().size()){}

		virtual void execute();

};

#endif
