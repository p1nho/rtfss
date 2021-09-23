#include "arch_topl_traverse.h"
#include <memory>

void arch_topl_traverse::execute(){
	std::deque<std::shared_ptr<arch_node>> travel_queue;
	std::vector<bool> visited(nodes.size(),false);

	//Find begining nodes
	
	//For each node
	for(auto n=nodes.begin();n!=nodes.end();n++){
		//Check if is begining 
		if(inv_adj_list[n->second].size()==0 //Inlist=empty => starting node
			||(inv_adj_list[n->second].size()==1 //But, if size is 1 and the curr node is
			&&n->second->get_op()==arch_node::INS)){ //a INS, then let in
			//Place on the queue
			travel_queue.push_front(n->second);
		}
	}

	//Traverse
	while(travel_queue.size()>0){
		//Remove node from travel_queue
		std::shared_ptr<arch_node> to_solve(travel_queue.back());
		travel_queue.pop_back();

		//Call solve on node
		handle_solve(to_solve);

		//Add to visited
		visited[to_solve->get_node_id()]=true;

		//Find more nodes to visit
		//For each (still not touched) node
		std::vector<std::shared_ptr<arch_node>> targets(adj_list[to_solve]);
		for(auto n=targets.begin();n!=targets.end();n++){
			//Check if not already visited
			if(visited[(*n)->get_node_id()]){
				continue;
			}

			std::vector<std::shared_ptr<arch_node>> rev_targets(inv_adj_list[*n]);
			bool add(true);
			for(auto c=rev_targets.begin();c!=rev_targets.end();c++){
				if(!visited[(*c)->get_node_id()]){
					add=false;
					break;
				}
			}
			if(add){
				travel_queue.push_front(*n);
			}
		}
	}
}

void arch_topl_traverse::handle_solve(std::shared_ptr<arch_node> node){

	switch(node->get_op()){

		case arch_node::ADD:
			solve_add(std::static_pointer_cast<arch_add>(node));
			break;			
		case arch_node::SUB: 
			solve_sub(std::static_pointer_cast<arch_sub>(node));
			break;			
		case arch_node::SIM:
			solve_sim(std::static_pointer_cast<arch_sim>(node));
			break;			
		case arch_node::MULT:
			solve_mult(std::static_pointer_cast<arch_mult>(node));
			break;			
		case arch_node::DIV:
			solve_div(std::static_pointer_cast<arch_div>(node));
			break;			
		case arch_node::SHL:
			solve_shl(std::static_pointer_cast<arch_shl>(node));
			break;			
		case arch_node::SHR:            
			solve_shr(std::static_pointer_cast<arch_shr>(node));
			break;			
		case arch_node::RTL:
			solve_rtl(std::static_pointer_cast<arch_rtl>(node));
			break;			
		case arch_node::RTR:            
			solve_rtr(std::static_pointer_cast<arch_rtr>(node));
			break;			
		case arch_node::AND:
			solve_and(std::static_pointer_cast<arch_and>(node));
			break;			
		case arch_node::OR:            
			solve_or(std::static_pointer_cast<arch_or>(node));
			break;			
		case arch_node::XOR:
			solve_xor(std::static_pointer_cast<arch_xor>(node));
			break;			
		case arch_node::NOT:            
			solve_not(std::static_pointer_cast<arch_not>(node));
			break;			
		//case arch_node::GAP:  		
		//	solve_gap(std::static_pointer_cast<arch_gap>(node));
		//	break;			
		case arch_node::COMP:           
			solve_comp(std::static_pointer_cast<arch_comp>(node));
			break;			
		//case arch_node::PROP:         
		//	solve_prop(std::static_pointer_cast<arch_prop>(node));
		//	break;			
		//case arch_node::VALINS:         
		//	solve_valins(std::static_pointer_cast<arch_valins>(node));
		//	break;			
		case arch_node::MOD:            
			solve_mod(std::static_pointer_cast<arch_mod>(node));
			break;			
		case arch_node::CAST:
			solve_cast(std::static_pointer_cast<arch_cast>(node));
			break;			
		case arch_node::NUM:            
			solve_num(std::static_pointer_cast<arch_num>(node));
			break;			
		case arch_node::ID: 
			solve_id(std::static_pointer_cast<arch_id>(node));
			break;			
		case arch_node::INS: 
			solve_ins(std::static_pointer_cast<arch_ins>(node));
			break;			
		/* This makes the compiler warn for uncompletenes
		default:
			std::cerr<<"arch_topl_traverse: UNKNOWN ARCH OP TYPE"<<std::endl;
			break;
		*/
	}
}
