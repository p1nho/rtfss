#ifndef RTFSS_DEF_ARCH_GEN
#define RTFSS_DEF_ARCH_GEN

#include "../ast/ast.h"
#include "../ast/ast_property.h"
#include "../ast/ast_visitor.h"
#include "../var_solver/var_solver.h"
#include "arch_graph.h"
#include "arch_node.h"
#include "../error_handler/error_handler.h"
#include "../error_handler/warning_handler.h"

#include <boost/dynamic_bitset/dynamic_bitset.hpp>
#include <cassert>
#include <memory>
#include <vector>
#include <set>
#include <cmath>
#include <boost/dynamic_bitset.hpp>

class arch_gen : public ast_visitor{

	public:

		arch_gen(std::shared_ptr<ast> tree,
				ast_property<unsigned int> &var_id,
				std::shared_ptr<var_complete_scope> vcscope,
				std::shared_ptr<pulse_complete_scope> pcscope,
				arch_graph& graph):
			ast_visitor(tree,true,true),
				var_id(var_id),
				vcscope(vcscope),
				pcscope(pcscope),
				inst_history(vcscope->get().size()),
				graph(graph),
				var_cnt(vcscope->get().size())
				{}

	private:

		ast_property<unsigned int> &var_id;
		std::shared_ptr<var_complete_scope> vcscope;
		std::shared_ptr<pulse_complete_scope> pcscope;

		ast_property<datatype> dominant_dt;
		std::vector<std::vector<std::shared_ptr<arch_ins>>> inst_history;

		ast_property<std::shared_ptr<arch_node>> result;
		arch_graph& graph;

		unsigned int var_cnt;

		boost::dynamic_bitset<> ins_fixed_point(std::shared_ptr<ast_num> n,datatype dom_dt,datatype& ret_dt,std::string name);

	protected:

		virtual void visit_start(std::shared_ptr<ast_start> node);
		virtual void visit_include(std::shared_ptr<ast_include> node);
		virtual void visit_cblk_decl(std::shared_ptr<ast_cblk_decl> node);
		virtual void visit_cblk_inst(std::shared_ptr<ast_cblk_inst> node);
		virtual void visit_s_cblk_inst(std::shared_ptr<ast_s_cblk_inst> node);
		virtual void visit_pulse(std::shared_ptr<ast_pulse> node);
		virtual void visit_if(std::shared_ptr<ast_if> node);
		virtual void visit_for(std::shared_ptr<ast_for> node);
		virtual void visit_decl(std::shared_ptr<ast_decl> node);
		virtual void visit_assign(std::shared_ptr<ast_assign> node);
		virtual void visit_add(std::shared_ptr<ast_add> node);
		virtual void visit_sub(std::shared_ptr<ast_sub> node);
		virtual void visit_sim(std::shared_ptr<ast_sim> node);
		virtual void visit_mult(std::shared_ptr<ast_mult> node);
		virtual void visit_div(std::shared_ptr<ast_div> node);
		virtual void visit_shl(std::shared_ptr<ast_shl> node);
		virtual void visit_shr(std::shared_ptr<ast_shr> node);
		virtual void visit_rtl(std::shared_ptr<ast_rtl> node);
		virtual void visit_rtr(std::shared_ptr<ast_rtr> node);
		virtual void visit_and(std::shared_ptr<ast_and> node);
		virtual void visit_or(std::shared_ptr<ast_or> node);
		virtual void visit_xor(std::shared_ptr<ast_xor> node);
		virtual void visit_not(std::shared_ptr<ast_not> node);
		virtual void visit_gap(std::shared_ptr<ast_gap> node);
		virtual void visit_comp(std::shared_ptr<ast_comp> node);
		virtual void visit_prop(std::shared_ptr<ast_prop> node);
		virtual void visit_mod(std::shared_ptr<ast_mod> node);
		virtual void visit_cast(std::shared_ptr<ast_cast> node);
		virtual void visit_arrinit(std::shared_ptr<ast_arrinit> node);
		virtual void visit_num(std::shared_ptr<ast_num> node);
		virtual void visit_id(std::shared_ptr<ast_id> node);

		virtual void visit_bin(std::shared_ptr<ast_bin_arith> node);
		virtual void visit_unary(std::shared_ptr<ast_unary_arith> node);


};

#endif
