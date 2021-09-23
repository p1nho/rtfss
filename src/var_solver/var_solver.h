#ifndef RTFSS_DEF_VAR_SOLVER
#define RTFSS_DEF_VAR_SOLVER

#include "../ast/ast.h"
#include "../ast/ast_visitor.h"
#include "../ast/ast_property.h"
#include "var.h"
#include "../error_handler/error_handler.h"
#include <cassert>

//TODO refactor var_id

class var_solver : public ast_visitor{

	public:
		var_solver(std::shared_ptr<ast> tree,
				ast_property<unsigned int> &var_id,
				ast_property<std::shared_ptr<var_scope>> &vscopes,
				ast_property<unsigned int> &pulse_id,
				ast_property<std::shared_ptr<pulse_scope>> &pscopes):
			ast_visitor(tree,false,false),
				var_id(var_id),
				vscopes(vscopes),
				pulse_id(pulse_id),
				pscopes(pscopes)
				{}

	private:

		ast_property<unsigned int> &var_id; //Registers the var associated with each possible node
		ast_property<std::shared_ptr<var_scope>> &vscopes;

		ast_property<unsigned int> &pulse_id; //Registers the pulse associated with each possible node

		ast_property<std::shared_ptr<pulse_scope>> &pscopes;

		ast_property<std::string> cblk_name; //Temporary, used to construct vscopes

		void sanity_test_vars(std::shared_ptr<var_complete_scope> vcs);
		void sanity_test_pulses(std::shared_ptr<pulse_complete_scope> pcs);

		std::shared_ptr<pulse_property> declare_pulse(std::shared_ptr<pulse_scope> ps,std::string& name);
		std::shared_ptr<pulse_property> use_pulse(std::shared_ptr<pulse_scope> ps,std::string& name);

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
