#include "frepr_rev_breath_traverse.h"

void frepr_rev_breath_traverse::execute(){

	std::queue<std::shared_ptr<frepr_node>> travel_queue;
	std::vector<std::shared_ptr<frepr_node>> traveled;
	std::vector<std::shared_ptr<frepr_node>> tmp_ins;

	tmp_ins=fanchor.get_ins();
	for(auto it=tmp_ins.begin();it!=tmp_ins.end();it++){
		travel_queue.push(*it);
	}

	while(travel_queue.size()!=0){
		std::shared_ptr<frepr_node> to_visit(travel_queue.front());
		travel_queue.pop();

		//Check if already visited
		auto ciav(std::find(traveled.begin(),traveled.end(),to_visit));
		if(ciav==traveled.end()){
			traveled.push_back(to_visit);

			handle_solve(to_visit);

			tmp_ins=to_visit->get_ins();
			for(auto it=tmp_ins.begin();it!=tmp_ins.end();it++){
				travel_queue.push(*it);
			}
		}
	}
}

void frepr_rev_breath_traverse::handle_solve(std::shared_ptr<frepr_node> node){

	switch(node->get_op()){

		case frepr_node::ANCHOR: 
			//Should NEVER be visited
			assert(false);
			break;			
		case frepr_node::CLOCK: 
			solve_clock(std::static_pointer_cast<frepr_clock>(node));
			break;			
		case frepr_node::FFD: 
			solve_ffd(std::static_pointer_cast<frepr_ffd>(node));
			break;			
		case frepr_node::PULSER: 
			solve_pulser(std::static_pointer_cast<frepr_pulser>(node));
			break;			
		case frepr_node::ADD:
			solve_add(std::static_pointer_cast<frepr_add>(node));
			break;			
		case frepr_node::SUB: 
			solve_sub(std::static_pointer_cast<frepr_sub>(node));
			break;			
		case frepr_node::SIM:
			solve_sim(std::static_pointer_cast<frepr_sim>(node));
			break;			
		case frepr_node::MULT:
			solve_mult(std::static_pointer_cast<frepr_mult>(node));
			break;			
		case frepr_node::DIV:
			solve_div(std::static_pointer_cast<frepr_div>(node));
			break;			
		case frepr_node::SHL:
			solve_shl(std::static_pointer_cast<frepr_shl>(node));
			break;			
		case frepr_node::SHR:            
			solve_shr(std::static_pointer_cast<frepr_shr>(node));
			break;			
		case frepr_node::RTL:
			solve_rtl(std::static_pointer_cast<frepr_rtl>(node));
			break;			
		case frepr_node::RTR:            
			solve_rtr(std::static_pointer_cast<frepr_rtr>(node));
			break;			
		case frepr_node::AND:
			solve_and(std::static_pointer_cast<frepr_and>(node));
			break;			
		case frepr_node::OR:            
			solve_or(std::static_pointer_cast<frepr_or>(node));
			break;			
		case frepr_node::XOR:
			solve_xor(std::static_pointer_cast<frepr_xor>(node));
			break;			
		case frepr_node::NOT:            
			solve_not(std::static_pointer_cast<frepr_not>(node));
			break;			
		case frepr_node::COMP:           
			solve_comp(std::static_pointer_cast<frepr_comp>(node));
			break;			
		case frepr_node::MOD:            
			solve_mod(std::static_pointer_cast<frepr_mod>(node));
			break;			
		case frepr_node::RESIZE:            
			solve_resize(std::static_pointer_cast<frepr_resize>(node));
			break;			
		case frepr_node::CONST:            
			solve_const(std::static_pointer_cast<frepr_const>(node));
			break;			
		/* This makes the compiler warn for uncompletenes
		default:
			std::cerr<<"frepr_topl_traverse: UNKNOWN ARCH OP TYPE"<<std::endl;
			break;
		*/
	}
}
