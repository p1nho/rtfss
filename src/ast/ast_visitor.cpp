#include "ast_visitor.h"
#include <memory>

void ast_visitor::visit(std::shared_ptr<ast> node){

	switch(node->get_op()){

		case ast::START:			//Start Node	
			visit_start(std::static_pointer_cast<ast_start>(node));
			break;

		case ast::INCLUDE:		//Include/Use Statement
			visit_include(std::static_pointer_cast<ast_include>(node));
			break;

		case ast::CBLK_DECL:		//Cblock declaration
			visit_cblk_decl(std::static_pointer_cast<ast_cblk_decl>(node));
			break;

		case ast::CBLK_INST:		//Cblock instantiation
			visit_cblk_inst(std::static_pointer_cast<ast_cblk_inst>(node));
			break;

		case ast::S_CBLK_INST:	//Short Cblock instantiation (inline on expr)
			visit_s_cblk_inst(std::static_pointer_cast<ast_s_cblk_inst>(node));
			break;

		case ast::PULSE:			//Pulse declaration
			visit_pulse(std::static_pointer_cast<ast_pulse>(node));
			break;

		case ast::IF:			//IF statement
			visit_if(std::static_pointer_cast<ast_if>(node));
			break;

		case ast::FOR:			//FOR statement
			visit_for(std::static_pointer_cast<ast_for>(node));
			break;

		case ast::DECL:			//Const/Stream declaration
			visit_decl(std::static_pointer_cast<ast_decl>(node));
			break;

		case ast::ASSIGN:		//Const/Stream assignment
			visit_assign(std::static_pointer_cast<ast_assign>(node));
			break;

		case ast::CAST:			//Casting
			visit_cast(std::static_pointer_cast<ast_cast>(node));
			break;

		case ast::ARRINIT:		//Const array initialization
			visit_arrinit(std::static_pointer_cast<ast_arrinit>(node));
			break;

		case ast::NUM:			//Numerical
			visit_num(std::static_pointer_cast<ast_num>(node));
			break;

		case ast::ID:			//Const/Stream identifier
			visit_id(std::static_pointer_cast<ast_id>(node));
			break;

		//Binary ops
		case ast::ADD:			//Addition
			if(!individual_bin)
				visit_bin(std::static_pointer_cast<ast_bin_arith>(node));
			else
				visit_add(std::static_pointer_cast<ast_add>(node));
			break;

		case ast::SUB:			//Subtraction
			if(!individual_bin)
				visit_bin(std::static_pointer_cast<ast_bin_arith>(node));
			else
				visit_sub(std::static_pointer_cast<ast_sub>(node));
			break;

		case ast::MULT:			//Multiplication
			if(!individual_bin)
				visit_bin(std::static_pointer_cast<ast_bin_arith>(node));
			else
				visit_mult(std::static_pointer_cast<ast_mult>(node));
			break;

		case ast::DIV:			//Division
			if(!individual_bin)
				visit_bin(std::static_pointer_cast<ast_bin_arith>(node));
			else
				visit_div(std::static_pointer_cast<ast_div>(node));
			break;

		case ast::SHL:			//Shift left
			if(!individual_bin)
				visit_bin(std::static_pointer_cast<ast_bin_arith>(node));
			else
				visit_shl(std::static_pointer_cast<ast_shl>(node));
			break;

		case ast::SHR:			//Shift right
			if(!individual_bin)
				visit_bin(std::static_pointer_cast<ast_bin_arith>(node));
			else
				visit_shr(std::static_pointer_cast<ast_shr>(node));
			break;

		case ast::RTL:			//Rotate left
			if(!individual_bin)
				visit_bin(std::static_pointer_cast<ast_bin_arith>(node));
			else
				visit_rtl(std::static_pointer_cast<ast_rtl>(node));
			break;

		case ast::RTR:			//Rotate right
			if(!individual_bin)
				visit_bin(std::static_pointer_cast<ast_bin_arith>(node));
			else
				visit_rtr(std::static_pointer_cast<ast_rtr>(node));
			break;

		case ast::AND:			//And operator
			if(!individual_bin)
				visit_bin(std::static_pointer_cast<ast_bin_arith>(node));
			else
				visit_and(std::static_pointer_cast<ast_and>(node));
			break;

		case ast::OR:			//Or operator
			if(!individual_bin)
				visit_bin(std::static_pointer_cast<ast_bin_arith>(node));
			else
				visit_or(std::static_pointer_cast<ast_or>(node));
			break;

		case ast::XOR:			//Xor operator
			if(!individual_bin)
				visit_bin(std::static_pointer_cast<ast_bin_arith>(node));
			else
				visit_xor(std::static_pointer_cast<ast_xor>(node));
			break;

		case ast::NOT:			//Not operator
			if(!individual_unary)
				visit_unary(std::static_pointer_cast<ast_unary_arith>(node));
			else
				visit_not(std::static_pointer_cast<ast_not>(node));
			break;

		case ast::GAP:			//Gap operation
			if(!individual_bin)
				visit_bin(std::static_pointer_cast<ast_bin_arith>(node));
			else
				visit_gap(std::static_pointer_cast<ast_gap>(node));
			break;

		case ast::COMP:			//Logic Comparison
			if(!individual_bin)
				visit_bin(std::static_pointer_cast<ast_bin_arith>(node));
			else
				visit_comp(std::static_pointer_cast<ast_comp>(node));
			break;

		case ast::MOD:			//Modulo operation
			if(!individual_bin)
				visit_bin(std::static_pointer_cast<ast_bin_arith>(node));
			else
				visit_mod(std::static_pointer_cast<ast_mod>(node));
			break;

		//Unary ops
		case ast::SIM:			//Simetric
			if(!individual_unary)
				visit_unary(std::static_pointer_cast<ast_unary_arith>(node));
			else
				visit_sim(std::static_pointer_cast<ast_sim>(node));
			break;

		case ast::PROP:			//"Property of" operation
			if(!individual_unary)
				visit_unary(std::static_pointer_cast<ast_unary_arith>(node));
			else
				visit_prop(std::static_pointer_cast<ast_prop>(node));
			break;

		/* This makes the compiler warn for uncompleteness
		default:
			std::cerr<<"AST_VISITOR UNKNOWN OP"<<std::endl;

		*/
			//TODO BITWISE LOGIC OPS 
	}

}

void ast_visitor::visit(std::vector<std::shared_ptr<ast>> nodes){

	for(auto it=nodes.begin();it<nodes.end();it++){
		visit(*it);
	}

}

void ast_visitor::execute(){
	visit(tree);
}

/* AST VISITOR BASE IMPLEMENTATION:

void my_visitor::visit_start(std::shared_ptr<ast_start> node){ visit(node->get_children()); }
void my_visitor::visit_include(std::shared_ptr<ast_include> node){ visit(node->get_children()); }
void my_visitor::visit_cblk_decl(std::shared_ptr<ast_cblk_decl> node){ visit(node->get_children()); }
void my_visitor::visit_cblk_inst(std::shared_ptr<ast_cblk_inst> node){ visit(node->get_children()); }
void my_visitor::visit_s_cblk_inst(std::shared_ptr<ast_s_cblk_inst> node){ visit(node->get_children()); }
void my_visitor::visit_pulse(std::shared_ptr<ast_pulse> node){ visit(node->get_children()); }
void my_visitor::visit_if(std::shared_ptr<ast_if> node){ visit(node->get_children()); }
void my_visitor::visit_for(std::shared_ptr<ast_for> node){ visit(node->get_children()); }
void my_visitor::visit_decl(std::shared_ptr<ast_decl> node){ visit(node->get_children()); }
void my_visitor::visit_assign(std::shared_ptr<ast_assign> node){ visit(node->get_children()); }
void my_visitor::visit_cast(std::shared_ptr<ast_cast> node){ visit(node->get_children()); }
void my_visitor::visit_arrinit(std::shared_ptr<ast_arrinit> node){ visit(node->get_children()); }
void my_visitor::visit_num(std::shared_ptr<ast_num> node){ visit(node->get_children()); }
void my_visitor::visit_id(std::shared_ptr<ast_id> node){ visit(node->get_children()); }

//Binary/Unary
void my_visitor::visit_add(std::shared_ptr<ast_add> node){ visit(node->get_children()); }
void my_visitor::visit_sub(std::shared_ptr<ast_sub> node){ visit(node->get_children()); }
void my_visitor::visit_sim(std::shared_ptr<ast_sim> node){ visit(node->get_children()); }
void my_visitor::visit_mult(std::shared_ptr<ast_mult> node){ visit(node->get_children()); }
void my_visitor::visit_div(std::shared_ptr<ast_div> node){ visit(node->get_children()); }
void my_visitor::visit_shl(std::shared_ptr<ast_shl> node){ visit(node->get_children()); }
void my_visitor::visit_shr(std::shared_ptr<ast_shr> node){ visit(node->get_children()); }
void my_visitor::visit_gap(std::shared_ptr<ast_gap> node){ visit(node->get_children()); }
void my_visitor::visit_comp(std::shared_ptr<ast_comp> node){ visit(node->get_children()); }
void my_visitor::visit_prop(std::shared_ptr<ast_prop> node){ visit(node->get_children()); }
void my_visitor::visit_mod(std::shared_ptr<ast_mod> node){ visit(node->get_children()); }

void my_visitor::visit_bin(std::shared_ptr<ast_bin_arith> node){ visit(node->get_children()); }
void my_visitor::visit_unary(std::shared_ptr<ast_unary_arith> node){ visit(node->get_children()); }

*/

