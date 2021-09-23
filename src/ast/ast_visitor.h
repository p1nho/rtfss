#ifndef RTFSS_DEF_AST_VISITOR
#define RTFSS_DEF_AST_VISITOR

#include "ast.h"

//TODO make template?
class ast_visitor{

		private:
			std::shared_ptr<ast> tree;
			bool individual_bin;
			bool individual_unary;

		public:
			void visit(std::shared_ptr<ast> node);
			void visit(std::vector<std::shared_ptr<ast>> nodes);
			void execute();

		protected:
			ast_visitor(std::shared_ptr<ast> tree,bool individual_bin,bool individual_unary):
				tree(tree),
				individual_bin(individual_bin),
				individual_unary(individual_unary){}

			virtual void visit_start(std::shared_ptr<ast_start> node)=0;
			virtual void visit_include(std::shared_ptr<ast_include> node)=0;
			virtual void visit_cblk_decl(std::shared_ptr<ast_cblk_decl> node)=0;
			virtual void visit_cblk_inst(std::shared_ptr<ast_cblk_inst> node)=0;
			virtual void visit_s_cblk_inst(std::shared_ptr<ast_s_cblk_inst> node)=0;
			virtual void visit_pulse(std::shared_ptr<ast_pulse> node)=0;
			virtual void visit_if(std::shared_ptr<ast_if> node)=0;
			virtual void visit_for(std::shared_ptr<ast_for> node)=0;
			virtual void visit_decl(std::shared_ptr<ast_decl> node)=0;
			virtual void visit_assign(std::shared_ptr<ast_assign> node)=0;
			virtual void visit_add(std::shared_ptr<ast_add> node)=0;
			virtual void visit_sub(std::shared_ptr<ast_sub> node)=0;
			virtual void visit_sim(std::shared_ptr<ast_sim> node)=0;
			virtual void visit_mult(std::shared_ptr<ast_mult> node)=0;
			virtual void visit_div(std::shared_ptr<ast_div> node)=0;
			virtual void visit_shl(std::shared_ptr<ast_shl> node)=0;
			virtual void visit_shr(std::shared_ptr<ast_shr> node)=0;
			virtual void visit_rtl(std::shared_ptr<ast_rtl> node)=0;
			virtual void visit_rtr(std::shared_ptr<ast_rtr> node)=0;
			virtual void visit_and(std::shared_ptr<ast_and> node)=0;
			virtual void visit_or(std::shared_ptr<ast_or> node)=0;
			virtual void visit_xor(std::shared_ptr<ast_xor> node)=0;
			virtual void visit_not(std::shared_ptr<ast_not> node)=0;
			virtual void visit_gap(std::shared_ptr<ast_gap> node)=0;
			virtual void visit_comp(std::shared_ptr<ast_comp> node)=0;
			virtual void visit_prop(std::shared_ptr<ast_prop> node)=0;
			virtual void visit_mod(std::shared_ptr<ast_mod> node)=0;
			virtual void visit_cast(std::shared_ptr<ast_cast> node)=0;
			virtual void visit_arrinit(std::shared_ptr<ast_arrinit> node)=0;
			virtual void visit_num(std::shared_ptr<ast_num> node)=0;
			virtual void visit_id(std::shared_ptr<ast_id> node)=0;

			virtual void visit_bin(std::shared_ptr<ast_bin_arith> node)=0;
			virtual void visit_unary(std::shared_ptr<ast_unary_arith> node)=0;


};

#endif
