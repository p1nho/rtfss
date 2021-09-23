#include "frepr_gen.h"
#include <boost/dynamic_bitset/dynamic_bitset.hpp>
#include <memory>
#include <utility>

//TODO
//Early implementation: each operation has a pipeline stage
//Timings should be calculated to expand or contract stages
void frepr_gen::execute(){
	std::map<unsigned int,std::shared_ptr<arch_node>> &nodes(graph.get_nodes());
	//std::map<std::shared_ptr<arch_node>,std::vector<std::shared_ptr<arch_node>>,std::owner_less<std::shared_ptr<arch_node>>> &inv_adj_list(graph.get_inv_adj_list());

	//Init node_cnt
	node_cnt=nodes.size();

	//Master clock node and add to anchor
	mclock=std::make_shared<frepr_clock>();
	fanchor.add_end_node(mclock);

	//Create pulser nodes
	create_pulse_nodes();

	//Call traversal
	arch_topl_traverse::execute();

	//Fix ins/ffd through ins_map
	for(auto ins=ins_map.begin();ins!=ins_map.end();ins++){
		std::shared_ptr<frepr_arith> target(std::static_pointer_cast<frepr_arith>(*gen_frepr.get(ins->second)));

		//Close the pseudo cycle
		ins->first->set_src(target);

		boost::dynamic_bitset<> defv(ins->first->get_default_val());
		if(!defv.empty()){ //Shift back!
			if(target->get_op()==frepr_node::FFD){
				std::static_pointer_cast<frepr_ffd>(target)->set_default_val(defv);
			}
			else{ //Is a INS
				std::static_pointer_cast<frepr_ffd>(target)->set_default_val(defv);
			}
			ins->first->set_default_val(boost::dynamic_bitset<>()); //Reset current
		}
	}

	//Add id endpoints to anchor, by traversing the nodes and finding arch_nodes with OUT
	std::map<unsigned int,std::shared_ptr<arch_node>> &arch_nodes(graph.get_nodes());
	for(auto n=arch_nodes.begin();n!=arch_nodes.end();n++){
		if(n->second->get_op()!=arch_node::ID) continue;
		std::shared_ptr<arch_id> nid(std::static_pointer_cast<arch_id>(n->second));

		if(nid->get_vk()==var_property::OUT){ //end node!
			fanchor.add_end_node(*(gen_frepr.get(nid)));
		}
	}

	//Tie pulse chains
	auto itpc=pulse_chain.begin();
	auto itpn=pulse_nodes.begin();
	for(;itpc!=pulse_chain.end()&&itpn!=pulse_nodes.end();itpc++,itpn++){
		std::shared_ptr<frepr_node> last=*itpn;

		//Travel this pulse's chain, and link
		for(auto ch=itpc->begin();ch!=itpc->end();ch++){
			//(*ch)->set_en();	Unset means always on
			(*ch)->set_clk(mclock);
			(*ch)->set_src(last);

			last=*ch;
		}

		//No need to anchor last node, since its already connected to other nodes 
	}

	//std::cerr<<"anchor size "<<fanchor.get_nodes().size()<<std::endl;
}

void frepr_gen::create_pulse_nodes(){

	unsigned int pc=0;
	for(auto p=pcscope->get().begin();p!=pcscope->get().end();p++,pc++){
		//TODO handle CBLOCK pulses
		switch((*p)->pid.get_pulse_type()){
			case pulse_id::NORMAL:
				{
					std::shared_ptr<frepr_pulser> np(std::make_shared<frepr_pulser>(node_cnt++));	
					np->set_ts((*p)->ts);
					np->set_val((*p)->pulse_val);
					pulse_nodes[pc]=np;
					break;
				}
			case pulse_id::CONST:
				//TODO hardwired workaround...
				{
					std::shared_ptr<frepr_pulser> np(std::make_shared<frepr_pulser>(node_cnt++));	
					np->set_ts(time_scale(time_scale::HERTZ));
					np->set_val(0);
					pulse_nodes[pc]=np;
					break;
				}
				
			case pulse_id::MAX:
				//TODO need to think about this more
				//TODO hardwired workaround...
				{
					std::shared_ptr<frepr_pulser> np(std::make_shared<frepr_pulser>(node_cnt++));	
					np->set_ts(time_scale(time_scale::HERTZ));
					np->set_val(0);
					pulse_nodes[pc]=np;
					break;
				}

			case pulse_id::PUNDEF:
				assert(false);
				break;
		}
	}
}

std::shared_ptr<frepr_ffd> frepr_gen::get_pulse(std::pair<unsigned int,unsigned int> *past_p,unsigned int pid){
	std::vector<std::shared_ptr<frepr_ffd>> &this_pulse(pulse_chain[pid]);

	if(past_p->first==pid){ //Same pulse
		if(past_p->second+1<this_pulse.size()){ //Already generated, reuse node
			std::shared_ptr<frepr_ffd> ffd(this_pulse[past_p->second+1]);
			past_p->second++; //Increment level
			return ffd; //Return node
		}
		else{ //Generate new level
			datatype dt(datatype::UNSIGNED,1,0);
			std::string n("pulse_chain");
			var_property::var_kind vk(var_property::REGULAR);
			std::shared_ptr<frepr_ffd> ffd(std::make_shared<frepr_ffd>(node_cnt++,dt,n,vk));
			this_pulse.push_back(ffd);
			past_p->second++; //Increment level
			return ffd;
		}
	}
	else{ //pulse change 
		if(this_pulse.size()>=1){ //Already generated, reuse node
			std::shared_ptr<frepr_ffd> ffd(this_pulse[0]);
			past_p->first=pid; //Set pulse
			past_p->second=0; //Set level
			return ffd; //Return node
		}
		else{ //Generate level
			datatype dt(dt.UNSIGNED,1,0);
			std::string n("pulse_chain");
			var_property::var_kind vk(var_property::REGULAR);
			std::shared_ptr<frepr_ffd> ffd(std::make_shared<frepr_ffd>(node_cnt++,dt,n,vk));
			this_pulse.push_back(ffd);
			past_p->first=pid; //Set pulse
			past_p->second=0; //Set level
			return ffd;
		}
	}
}

std::shared_ptr<frepr_ffd> frepr_gen::start_chain(unsigned int pid){
	std::vector<std::shared_ptr<frepr_ffd>> &this_pulse(pulse_chain[pid]);
	if(this_pulse.size()>=1){ //Already generated, reuse node
		std::shared_ptr<frepr_ffd> ffd(this_pulse[0]);
		return ffd; //Return node
	}
	else{ //Generate level
		datatype dt(dt.UNSIGNED,1,0);
		std::string n("pulse_chain");
		var_property::var_kind vk(var_property::REGULAR);
		std::shared_ptr<frepr_ffd> ffd(std::make_shared<frepr_ffd>(node_cnt++,dt,n,vk));
		this_pulse.push_back(ffd);
		return ffd;
	}
}

std::pair<std::shared_ptr<frepr_arith>,std::shared_ptr<frepr_arith>> frepr_gen::resolve_bin(std::shared_ptr<arch_node> right,std::shared_ptr<arch_node> left,unsigned int pid,std::pair<std::shared_ptr<frepr_ffd>,std::pair<unsigned int,unsigned int>> &p_to_use){

	//Four cases:
	//0) left and right same pulse as current node
	//1) left same pulse as current node, right different
	//2) right same pulse as current node, left different
	//3) left and right different pulse than current node
	
	bool order;
	std::pair<unsigned int,unsigned int> rightp,leftp;
	rightp=*(curr_pulse.get(right));
	leftp=*(curr_pulse.get(left));

	//Case 0
	if(leftp.first==pid&&rightp.first==pid){
		//Find the pipeline with the most delay
		std::shared_ptr<arch_node> amost,aleast;		
		std::shared_ptr<frepr_arith> fmost,fleast;		
		std::pair<unsigned int,unsigned int> mostp,leastp;
		
		if(rightp.second>leftp.second){
			amost=right;	
			aleast=left;	

			mostp=rightp;
			leastp=leftp;

			order=true;
		}
		else{
			amost=left;	
			aleast=right;	

			mostp=leftp;
			leastp=rightp;

			order=false;
		}

		fmost=std::static_pointer_cast<frepr_arith>(*(gen_frepr.get(amost)));
		fleast=std::static_pointer_cast<frepr_arith>(*(gen_frepr.get(aleast)));

		//Add delay to the quickest path (least var)
		for(unsigned int i=leastp.second;i<mostp.second;i++){
			//Get pulse
			std::shared_ptr<frepr_ffd> pp(get_pulse(&leastp,pid));	

			//Create delay with flip flop d
			std::string vn("sync_bubble");
			var_property::var_kind vk(var_property::REGULAR);
			datatype dt_ffd(aleast->get_datatype());
			std::shared_ptr<frepr_ffd> nnode(std::make_shared<frepr_ffd>(node_cnt++,dt_ffd,vn,vk));
			nnode->set_en(pp);
			nnode->set_clk(mclock);	
			nnode->set_src(fleast);

			fleast=nnode;	
		}

		//Get final pulse
		p_to_use.first=get_pulse(&leastp,pid);
		p_to_use.second=leastp;

		//Return pair by order
		if(order){
			return std::make_pair(fleast,fmost);
		}
		else{
			return std::make_pair(fmost,fleast);
		}
	}	
	//Case 1
	//1) left same pulse as current node, right different
	else if(leftp.first==pid&&rightp.first!=pid){
		//Use left side as base, and ignore right pulse
		//No need to adjust delays

		std::shared_ptr<frepr_node> fleft,fright;		
		fleft=*(gen_frepr.get(left));
		fright=*(gen_frepr.get(right));

		p_to_use.first=get_pulse(&leftp,pid);
		p_to_use.second=leftp;

		return std::make_pair(std::static_pointer_cast<frepr_arith>(fleft),
				std::static_pointer_cast<frepr_arith>(fright));
	}
	//Case 2
	//2) right same pulse as current node, left different
	else if(rightp.first==pid&&leftp.first!=pid){
		//Use right side as base, and ignore left pulse
		//No need to adjust delays
		
		std::shared_ptr<frepr_node> fleft,fright;		
		fleft=*(gen_frepr.get(left));
		fright=*(gen_frepr.get(right));

		p_to_use.first=get_pulse(&rightp,pid);
		p_to_use.second=rightp;

		return std::make_pair(std::static_pointer_cast<frepr_arith>(fleft),
				std::static_pointer_cast<frepr_arith>(fright));
	}
	//Case 3
	//3) left and right different pulse than current node
	//else if(leftp.first!=pid&&rightp.first!=pid){
	else{
		//Use no pulse as base, start new chain
		//No need to adjust delays
		
		std::shared_ptr<frepr_node> fleft,fright;		
		fleft=*(gen_frepr.get(left));
		fright=*(gen_frepr.get(right));

		p_to_use.second=std::make_pair(pid,0); 
		p_to_use.first=start_chain(pid);
		
		return std::make_pair(std::static_pointer_cast<frepr_arith>(fleft),
				std::static_pointer_cast<frepr_arith>(fright));
	}
}

std::shared_ptr<frepr_arith> frepr_gen::align_stream(std::shared_ptr<frepr_arith> in,datatype& target){
	//The hardware designed in this function is purely bit wiring so, no delay or ffd is needed
	
	datatype currdt(in->get_datatype());
	unsigned int curr_size(currdt.get_integ()+currdt.get_frac());
	unsigned int target_size(target.get_integ()+target.get_frac());
	//std::cout<<"curr_size: "<<curr_size<<std::endl;
	//std::cout<<"target_size: "<<target_size<<std::endl;
	//std::cout<<"currdt: "<<currdt.to_string()<<std::endl;
	//std::cout<<"target: "<<target.to_string()<<std::endl;
	//std::cout<<std::endl;

	std::shared_ptr<frepr_resize> res;
	std::shared_ptr<frepr_bin_arith> shift;
	std::shared_ptr<frepr_const> shamt;
	std::shared_ptr<frepr_arith> nres,nshift,nshamt;
	unsigned int shamt_s;
	datatype shamt_dt;
	boost::dynamic_bitset<> shamt_v;
	if(target_size>curr_size){ //resize then shift
		res=std::make_shared<frepr_resize>(node_cnt++,target);
		res->set_target(in);
		nres=std::static_pointer_cast<frepr_arith>(res);

		if(target.get_frac()>currdt.get_frac()){ //shift left
			shamt_s=std::ceil(std::log2(target.get_frac()-currdt.get_frac()+1));
			shamt_dt=datatype(datatype::UNSIGNED,shamt_s,0);
			shamt_v=boost::dynamic_bitset<>(shamt_s,target.get_frac()-currdt.get_frac());
			shamt=std::make_shared<frepr_const>(node_cnt++,shamt_dt,shamt_v);
			nshamt=std::static_pointer_cast<frepr_arith>(shamt);

			shift=std::make_shared<frepr_shl>(node_cnt++,target);
			shift->set_left(nres);
			shift->set_right(nshamt);

			return shift;
		}
		else if(target.get_frac()<currdt.get_frac()){ //shift right
			shamt_s=std::ceil(std::log2(currdt.get_frac()-target.get_frac()+1));
			shamt_dt=datatype(datatype::UNSIGNED,shamt_s,0);
			shamt_v=boost::dynamic_bitset<>(shamt_s,currdt.get_frac()-target.get_frac());
			shamt=std::make_shared<frepr_const>(node_cnt++,shamt_dt,shamt_v);
			nshamt=std::static_pointer_cast<frepr_arith>(shamt);

			shift=std::make_shared<frepr_shr>(node_cnt++,target);
			shift->set_left(nres);
			shift->set_right(nshamt);

			return shift;
		}
		else{ //dont shift, only resize
			return res;
		}
	}
	else if(target_size<curr_size){ //shift then resize

		if(target.get_frac()>currdt.get_frac()){ //shift left
			shamt_s=std::ceil(std::log2(target.get_frac()-currdt.get_frac()+1));
			shamt_dt=datatype(datatype::UNSIGNED,shamt_s,0);
			shamt_v=boost::dynamic_bitset<>(shamt_s,target.get_frac()-currdt.get_frac());
			shamt=std::make_shared<frepr_const>(node_cnt++,shamt_dt,shamt_v);
			nshamt=std::static_pointer_cast<frepr_arith>(shamt);

			shift=std::make_shared<frepr_shl>(node_cnt++,currdt);
			shift->set_left(in);
			shift->set_right(nshamt);
			nshift=std::static_pointer_cast<frepr_arith>(shift);
		}
		else if(target.get_frac()<currdt.get_frac()){ //shift right
			shamt_s=std::ceil(std::log2(currdt.get_frac()-target.get_frac()+1));
			shamt_dt=datatype(datatype::UNSIGNED,shamt_s,0);
			shamt_v=boost::dynamic_bitset<>(shamt_s,currdt.get_frac()-target.get_frac());
			shamt=std::make_shared<frepr_const>(node_cnt++,shamt_dt,shamt_v);
			nshamt=std::static_pointer_cast<frepr_arith>(shamt);

			shift=std::make_shared<frepr_shr>(node_cnt++,currdt);
			shift->set_left(in);
			shift->set_right(nshamt);
			nshift=std::static_pointer_cast<frepr_arith>(shift);
		}
		else{ //dont shift, only resize
			nshift=in; //Hack
		}

		res=std::make_shared<frepr_resize>(node_cnt++,target);
		res->set_target(nshift);
		return res;
	}
	else{ //do nothing, stream is already aligned
		return in;
	}
}

datatype frepr_gen::inclusive_datatype(datatype::dt_types ddt,datatype dt0,datatype dt1){
	//TODO pass to datatype decl in ast?
	unsigned int integ(std::max(dt0.get_integ(),dt1.get_integ()));
	unsigned int frac(std::max(dt0.get_frac(),dt1.get_frac()));
	return datatype(ddt,integ,frac);
}

void frepr_gen::solve_add(std::shared_ptr<arch_add> node){
	//std::cerr<<"add"<<" "<<node->get_node_id()<<std::endl;

	//Create fix bin operation
	std::shared_ptr<arch_node> right(node->get_right());
	std::shared_ptr<arch_node> left(node->get_left());
	unsigned int pid(node->get_pulse_id());
	std::pair<std::shared_ptr<frepr_ffd>,std::pair<unsigned int,unsigned int>> ret_p;
	std::pair<std::shared_ptr<frepr_arith>,std::shared_ptr<frepr_arith>> fixed_v(resolve_bin(right,left,pid,ret_p));	

	//Create node
	datatype dt(node->get_datatype());
	std::shared_ptr<frepr_add> nnode(std::make_shared<frepr_add>(node->get_node_id(),dt));
	std::shared_ptr<frepr_arith> fleft(align_stream(fixed_v.first,dt));
	std::shared_ptr<frepr_arith> fright(align_stream(fixed_v.second,dt));
	nnode->set_left(fleft);
	nnode->set_right(fright);

	//Create flipflop
	std::string bn("buffer");
	var_property::var_kind vk(var_property::REGULAR);
	std::shared_ptr<frepr_ffd> nnode_ffd(std::make_shared<frepr_ffd>(node_cnt++,dt,bn,vk));
	nnode_ffd->set_src(nnode);
	nnode_ffd->set_clk(mclock);
	nnode_ffd->set_en(ret_p.first);

	//Add to curr_pulse
	curr_pulse.put(node,ret_p.second);
	
	//Add to gen_frepr
	std::shared_ptr<frepr_node> gen_nnode_ffd(nnode_ffd);
	gen_frepr.put(node,gen_nnode_ffd);
}

void frepr_gen::solve_sub(std::shared_ptr<arch_sub> node){
	//std::cerr<<"sub"<<" "<<node->get_node_id()<<std::endl;

	//Create fix bin operation
	std::shared_ptr<arch_node> right(node->get_right());
	std::shared_ptr<arch_node> left(node->get_left());
	unsigned int pid(node->get_pulse_id());
	std::pair<std::shared_ptr<frepr_ffd>,std::pair<unsigned int,unsigned int>> ret_p;
	std::pair<std::shared_ptr<frepr_arith>,std::shared_ptr<frepr_arith>> fixed_v(resolve_bin(right,left,pid,ret_p));	

	//Create node
	datatype dt(node->get_datatype());
	std::shared_ptr<frepr_sub> nnode(std::make_shared<frepr_sub>(node->get_node_id(),dt));
	std::shared_ptr<frepr_arith> fleft(align_stream(fixed_v.first,dt));
	std::shared_ptr<frepr_arith> fright(align_stream(fixed_v.second,dt));
	nnode->set_left(fleft);
	nnode->set_right(fright);

	//Create flipflop
	std::string bn("buffer");
	var_property::var_kind vk(var_property::REGULAR);
	std::shared_ptr<frepr_ffd> nnode_ffd(std::make_shared<frepr_ffd>(node_cnt++,dt,bn,vk));
	nnode_ffd->set_src(nnode);
	nnode_ffd->set_clk(mclock);
	nnode_ffd->set_en(ret_p.first);

	//Add to curr_pulse
	curr_pulse.put(node,ret_p.second);
	
	//Add to gen_frepr
	std::shared_ptr<frepr_node> gen_nnode_ffd(nnode_ffd);
	gen_frepr.put(node,gen_nnode_ffd);

}

void frepr_gen::solve_sim(std::shared_ptr<arch_sim> node){
	//std::cerr<<"sim"<<" "<<node->get_node_id()<<std::endl;

	//Create curr_pulse associated to this branch
	std::pair<unsigned int,unsigned int> cp(*(curr_pulse.get(node->get_target())));
	
	std::shared_ptr<frepr_ffd> pulse_ffd(get_pulse(&cp,node->get_pulse_id()));			
	curr_pulse.put(node,cp);
	
	//Create node
	datatype dt(node->get_datatype());
	std::shared_ptr<frepr_sim> nnode(std::make_shared<frepr_sim>(node->get_node_id(),dt));
	std::shared_ptr<frepr_arith> target(std::static_pointer_cast<frepr_arith>(*gen_frepr.get(node->get_target())));
	std::shared_ptr<frepr_arith> ftarget(align_stream(target,dt));
	nnode->set_target(ftarget);

	//Create flipflop
	std::string bn("buffer");
	var_property::var_kind vk(var_property::REGULAR);
	std::shared_ptr<frepr_ffd> nnode_ffd(std::make_shared<frepr_ffd>(node_cnt++,dt,bn,vk));
	nnode_ffd->set_src(nnode);
	nnode_ffd->set_clk(mclock);
	nnode_ffd->set_en(pulse_ffd);
	
	//Add to gen_frepr
	std::shared_ptr<frepr_node> gen_nnode_ffd(nnode_ffd);
	gen_frepr.put(node,gen_nnode_ffd);
}

void frepr_gen::solve_mult(std::shared_ptr<arch_mult> node){
	//std::cerr<<"mult"<<" "<<node->get_node_id()<<std::endl;

	//Create fix bin operation
	std::shared_ptr<arch_node> right(node->get_right());
	std::shared_ptr<arch_node> left(node->get_left());
	unsigned int pid(node->get_pulse_id());
	std::pair<std::shared_ptr<frepr_ffd>,std::pair<unsigned int,unsigned int>> ret_p;
	std::pair<std::shared_ptr<frepr_arith>,std::shared_ptr<frepr_arith>> fixed_v(resolve_bin(right,left,pid,ret_p));	

	//Create node
	datatype dt(node->get_datatype());
	std::shared_ptr<frepr_mult> nnode(std::make_shared<frepr_mult>(node->get_node_id(),dt));
	std::shared_ptr<frepr_arith> fleft(align_stream(fixed_v.first,dt));
	std::shared_ptr<frepr_arith> fright(align_stream(fixed_v.second,dt));
	nnode->set_left(fleft);
	nnode->set_right(fright);

	//Create flipflop
	std::string bn("buffer");
	var_property::var_kind vk(var_property::REGULAR);
	std::shared_ptr<frepr_ffd> nnode_ffd(std::make_shared<frepr_ffd>(node_cnt++,dt,bn,vk));
	nnode_ffd->set_src(nnode);
	nnode_ffd->set_clk(mclock);
	nnode_ffd->set_en(ret_p.first);

	//Add to curr_pulse
	curr_pulse.put(node,ret_p.second);
	
	//Add to gen_frepr
	std::shared_ptr<frepr_node> gen_nnode_ffd(nnode_ffd);
	gen_frepr.put(node,gen_nnode_ffd);
}

void frepr_gen::solve_div(std::shared_ptr<arch_div> node){
	//std::cerr<<"div"<<" "<<node->get_node_id()<<std::endl;

	//Create fix bin operation
	std::shared_ptr<arch_node> right(node->get_right());
	std::shared_ptr<arch_node> left(node->get_left());
	unsigned int pid(node->get_pulse_id());
	std::pair<std::shared_ptr<frepr_ffd>,std::pair<unsigned int,unsigned int>> ret_p;
	std::pair<std::shared_ptr<frepr_arith>,std::shared_ptr<frepr_arith>> fixed_v(resolve_bin(right,left,pid,ret_p));	

	//Create node
	datatype dt(node->get_datatype());
	std::shared_ptr<frepr_div> nnode(std::make_shared<frepr_div>(node->get_node_id(),dt));
	std::shared_ptr<frepr_arith> fleft(align_stream(fixed_v.first,dt));
	std::shared_ptr<frepr_arith> fright(align_stream(fixed_v.second,dt));
	nnode->set_left(fleft);
	nnode->set_right(fright);

	//Create flipflop
	std::string bn("buffer");
	var_property::var_kind vk(var_property::REGULAR);
	std::shared_ptr<frepr_ffd> nnode_ffd(std::make_shared<frepr_ffd>(node_cnt++,dt,bn,vk));
	nnode_ffd->set_src(nnode);
	nnode_ffd->set_clk(mclock);
	nnode_ffd->set_en(ret_p.first);

	//Add to curr_pulse
	curr_pulse.put(node,ret_p.second);
	
	//Add to gen_frepr
	std::shared_ptr<frepr_node> gen_nnode_ffd(nnode_ffd);
	gen_frepr.put(node,gen_nnode_ffd);
}

void frepr_gen::solve_shl(std::shared_ptr<arch_shl> node){
	//std::cerr<<"shl"<<" "<<node->get_node_id()<<std::endl;

	//No delay operation
	
	//Get curr_pulse associated to the left branch and bind to this one
	std::pair<unsigned int,unsigned int> cp(*(curr_pulse.get(node->get_left())));
	curr_pulse.put(node,cp);
	
	std::shared_ptr<arch_node> left(node->get_left());
	std::shared_ptr<arch_node> right(node->get_right());

	std::shared_ptr<frepr_arith> fleft(std::static_pointer_cast<frepr_arith>(*gen_frepr.get(node->get_left())));
	std::shared_ptr<frepr_arith> fright(std::static_pointer_cast<frepr_arith>(*gen_frepr.get(node->get_right())));

	//Create node
	//Dont align left and right to output
	//Create big datatype that can represent left and final ouput
	//Do shifts there and then change back to final ouput datatype
	datatype dt(node->get_datatype());
	datatype dt_tmp(inclusive_datatype(dt.get_datatype(),dt,left->get_datatype()));

	fleft=align_stream(fleft,dt_tmp);
	//Dont need to align right
	
	std::shared_ptr<frepr_shl> nnode_tmp(std::make_shared<frepr_shl>(node->get_node_id(),dt_tmp));
	nnode_tmp->set_left(fleft);
	nnode_tmp->set_right(fright);

	//Create final node with the correct sizing
	std::shared_ptr<frepr_arith> nnode(align_stream(nnode_tmp,dt));
	
	//Add to gen_frepr
	std::shared_ptr<frepr_node> gen_nnode(nnode);
	gen_frepr.put(node,gen_nnode);
}

void frepr_gen::solve_shr(std::shared_ptr<arch_shr> node){
	//std::cerr<<"shr"<<" "<<node->get_node_id()<<std::endl;

	//No delay operation
	
	//Get curr_pulse associated to the left branch and bind to this one
	std::pair<unsigned int,unsigned int> cp(*(curr_pulse.get(node->get_left())));
	curr_pulse.put(node,cp);
	
	std::shared_ptr<arch_node> left(node->get_left());
	std::shared_ptr<arch_node> right(node->get_right());

	std::shared_ptr<frepr_arith> fleft(std::static_pointer_cast<frepr_arith>(*gen_frepr.get(node->get_left())));
	std::shared_ptr<frepr_arith> fright(std::static_pointer_cast<frepr_arith>(*gen_frepr.get(node->get_right())));

	//Create node
	//Dont align left and right to output
	//Create big datatype that can represent left and final ouput
	//Do shifts there and then change back to final ouput datatype
	datatype dt(node->get_datatype());
	datatype dt_tmp(inclusive_datatype(dt.get_datatype(),dt,left->get_datatype()));

	fleft=align_stream(fleft,dt_tmp);
	//Dont need to align right
	
	std::shared_ptr<frepr_shr> nnode_tmp(std::make_shared<frepr_shr>(node->get_node_id(),dt_tmp));
	nnode_tmp->set_left(fleft);
	nnode_tmp->set_right(fright);

	//Create final node with the correct sizing
	std::shared_ptr<frepr_arith> nnode(align_stream(nnode_tmp,dt));
	
	//Add to gen_frepr
	std::shared_ptr<frepr_node> gen_nnode(nnode);
	gen_frepr.put(node,gen_nnode);
}

void frepr_gen::solve_rtl(std::shared_ptr<arch_rtl> node){
	//std::cerr<<"rtl"<<" "<<node->get_node_id()<<std::endl;

	//No delay operation
	
	//Get curr_pulse associated to the left branch and bind to this one
	std::pair<unsigned int,unsigned int> cp(*(curr_pulse.get(node->get_left())));
	curr_pulse.put(node,cp);
	
	std::shared_ptr<arch_node> left(node->get_left());
	std::shared_ptr<arch_node> right(node->get_right());

	//Align left to output but dont align right!
	datatype dt(node->get_datatype());
	std::shared_ptr<frepr_arith> fleft(align_stream(std::static_pointer_cast<frepr_arith>(*gen_frepr.get(node->get_left())),dt));
	std::shared_ptr<frepr_arith> fright(std::static_pointer_cast<frepr_arith>(*gen_frepr.get(node->get_right())));

	//Create node
	std::shared_ptr<frepr_rtl> nnode(std::make_shared<frepr_rtl>(node->get_node_id(),dt));
	nnode->set_left(fleft);
	nnode->set_right(fright);

	//Add to gen_frepr
	std::shared_ptr<frepr_node> gen_nnode(nnode);
	gen_frepr.put(node,gen_nnode);
}

void frepr_gen::solve_rtr(std::shared_ptr<arch_rtr> node){
	//std::cerr<<"rtr"<<" "<<node->get_node_id()<<std::endl;

	//No delay operation
	
	//Get curr_pulse associated to the left branch and bind to this one
	std::pair<unsigned int,unsigned int> cp(*(curr_pulse.get(node->get_left())));
	curr_pulse.put(node,cp);
	
	std::shared_ptr<arch_node> left(node->get_left());
	std::shared_ptr<arch_node> right(node->get_right());

	//Align left to output but dont align right!
	datatype dt(node->get_datatype());
	std::shared_ptr<frepr_arith> fleft(align_stream(std::static_pointer_cast<frepr_arith>(*gen_frepr.get(node->get_left())),dt));
	std::shared_ptr<frepr_arith> fright(std::static_pointer_cast<frepr_arith>(*gen_frepr.get(node->get_right())));

	//Create node
	std::shared_ptr<frepr_rtr> nnode(std::make_shared<frepr_rtr>(node->get_node_id(),dt));
	nnode->set_left(fleft);
	nnode->set_right(fright);

	//Add to gen_frepr
	std::shared_ptr<frepr_node> gen_nnode(nnode);
	gen_frepr.put(node,gen_nnode);
}

void frepr_gen::solve_and(std::shared_ptr<arch_and> node){
	//std::cerr<<"and"<<" "<<node->get_node_id()<<std::endl;

	//Create fix bin operation
	std::shared_ptr<arch_node> right(node->get_right());
	std::shared_ptr<arch_node> left(node->get_left());
	unsigned int pid(node->get_pulse_id());
	std::pair<std::shared_ptr<frepr_ffd>,std::pair<unsigned int,unsigned int>> ret_p;
	std::pair<std::shared_ptr<frepr_arith>,std::shared_ptr<frepr_arith>> fixed_v(resolve_bin(right,left,pid,ret_p));	

	//Create node
	datatype dt(node->get_datatype());
	std::shared_ptr<frepr_and> nnode(std::make_shared<frepr_and>(node->get_node_id(),dt));
	std::shared_ptr<frepr_arith> fleft(align_stream(fixed_v.first,dt));
	std::shared_ptr<frepr_arith> fright(align_stream(fixed_v.second,dt));
	nnode->set_left(fleft);
	nnode->set_right(fright);

	//Create flipflop
	std::string bn("buffer");
	var_property::var_kind vk(var_property::REGULAR);
	std::shared_ptr<frepr_ffd> nnode_ffd(std::make_shared<frepr_ffd>(node_cnt++,dt,bn,vk));
	nnode_ffd->set_src(nnode);
	nnode_ffd->set_clk(mclock);
	nnode_ffd->set_en(ret_p.first);

	//Add to curr_pulse
	curr_pulse.put(node,ret_p.second);
	
	//Add to gen_frepr
	std::shared_ptr<frepr_node> gen_nnode_ffd(nnode_ffd);
	gen_frepr.put(node,gen_nnode_ffd);
}

void frepr_gen::solve_or(std::shared_ptr<arch_or> node){
	//std::cerr<<"or"<<" "<<node->get_node_id()<<std::endl;

	//Create fix bin operation
	std::shared_ptr<arch_node> right(node->get_right());
	std::shared_ptr<arch_node> left(node->get_left());
	unsigned int pid(node->get_pulse_id());
	std::pair<std::shared_ptr<frepr_ffd>,std::pair<unsigned int,unsigned int>> ret_p;
	std::pair<std::shared_ptr<frepr_arith>,std::shared_ptr<frepr_arith>> fixed_v(resolve_bin(right,left,pid,ret_p));	

	//Create node
	datatype dt(node->get_datatype());
	std::shared_ptr<frepr_or> nnode(std::make_shared<frepr_or>(node->get_node_id(),dt));
	std::shared_ptr<frepr_arith> fleft(align_stream(fixed_v.first,dt));
	std::shared_ptr<frepr_arith> fright(align_stream(fixed_v.second,dt));
	nnode->set_left(fleft);
	nnode->set_right(fright);

	//Create flipflop
	std::string bn("buffer");
	var_property::var_kind vk(var_property::REGULAR);
	std::shared_ptr<frepr_ffd> nnode_ffd(std::make_shared<frepr_ffd>(node_cnt++,dt,bn,vk));
	nnode_ffd->set_src(nnode);
	nnode_ffd->set_clk(mclock);
	nnode_ffd->set_en(ret_p.first);

	//Add to curr_pulse
	curr_pulse.put(node,ret_p.second);
	
	//Add to gen_frepr
	std::shared_ptr<frepr_node> gen_nnode_ffd(nnode_ffd);
	gen_frepr.put(node,gen_nnode_ffd);
}

void frepr_gen::solve_xor(std::shared_ptr<arch_xor> node){
	//std::cerr<<"xor"<<" "<<node->get_node_id()<<std::endl;

	//Create fix bin operation
	std::shared_ptr<arch_node> right(node->get_right());
	std::shared_ptr<arch_node> left(node->get_left());
	unsigned int pid(node->get_pulse_id());
	std::pair<std::shared_ptr<frepr_ffd>,std::pair<unsigned int,unsigned int>> ret_p;
	std::pair<std::shared_ptr<frepr_arith>,std::shared_ptr<frepr_arith>> fixed_v(resolve_bin(right,left,pid,ret_p));	

	//Create node
	datatype dt(node->get_datatype());
	std::shared_ptr<frepr_xor> nnode(std::make_shared<frepr_xor>(node->get_node_id(),dt));
	std::shared_ptr<frepr_arith> fleft(align_stream(fixed_v.first,dt));
	std::shared_ptr<frepr_arith> fright(align_stream(fixed_v.second,dt));
	nnode->set_left(fleft);
	nnode->set_right(fright);

	//Create flipflop
	std::string bn("buffer");
	var_property::var_kind vk(var_property::REGULAR);
	std::shared_ptr<frepr_ffd> nnode_ffd(std::make_shared<frepr_ffd>(node_cnt++,dt,bn,vk));
	nnode_ffd->set_src(nnode);
	nnode_ffd->set_clk(mclock);
	nnode_ffd->set_en(ret_p.first);

	//Add to curr_pulse
	curr_pulse.put(node,ret_p.second);
	
	//Add to gen_frepr
	std::shared_ptr<frepr_node> gen_nnode_ffd(nnode_ffd);
	gen_frepr.put(node,gen_nnode_ffd);
}

void frepr_gen::solve_not(std::shared_ptr<arch_not> node){
	//std::cerr<<"not"<<" "<<node->get_node_id()<<std::endl;

	//Create curr_pulse associated to this branch
	std::pair<unsigned int,unsigned int> cp(*(curr_pulse.get(node->get_target())));
	
	std::shared_ptr<frepr_ffd> pulse_ffd(get_pulse(&cp,node->get_pulse_id()));			
	curr_pulse.put(node,cp);
	
	//Create node
	datatype dt(node->get_datatype());
	std::shared_ptr<frepr_not> nnode(std::make_shared<frepr_not>(node->get_node_id(),dt));
	std::shared_ptr<frepr_arith> target(std::static_pointer_cast<frepr_arith>(*gen_frepr.get(node->get_target())));
	std::shared_ptr<frepr_arith> ftarget(align_stream(target,dt));
	nnode->set_target(ftarget);

	//Create flipflop
	std::string bn("buffer");
	var_property::var_kind vk(var_property::REGULAR);
	std::shared_ptr<frepr_ffd> nnode_ffd(std::make_shared<frepr_ffd>(node_cnt++,dt,bn,vk));
	nnode_ffd->set_src(nnode);
	nnode_ffd->set_clk(mclock);
	nnode_ffd->set_en(pulse_ffd);
	
	//Add to gen_frepr
	std::shared_ptr<frepr_node> gen_nnode_ffd(nnode_ffd);
	gen_frepr.put(node,gen_nnode_ffd);
}

void frepr_gen::solve_comp(std::shared_ptr<arch_comp> node){
	//std::cerr<<"comp"<<" "<<node->get_node_id()<<std::endl;

	//Create fix bin operation
	std::shared_ptr<arch_node> right(node->get_right());
	std::shared_ptr<arch_node> left(node->get_left());
	unsigned int pid(node->get_pulse_id());
	std::pair<std::shared_ptr<frepr_ffd>,std::pair<unsigned int,unsigned int>> ret_p;
	std::pair<std::shared_ptr<frepr_arith>,std::shared_ptr<frepr_arith>> fixed_v(resolve_bin(right,left,pid,ret_p));	

	//Create node
	//Dont align left and right to output, but to a temporary bigger datatype that can represent both datatypes
	datatype dt(node->get_datatype());
	datatype dt_tmp(inclusive_datatype(dt.get_datatype(),right->get_datatype(),left->get_datatype()));

	std::shared_ptr<frepr_comp> nnode_tmp(std::make_shared<frepr_comp>(node->get_node_id(),node->get_logic_op(),dt_tmp));
	std::shared_ptr<frepr_arith> fleft(align_stream(fixed_v.first,dt_tmp));
	std::shared_ptr<frepr_arith> fright(align_stream(fixed_v.second,dt_tmp));
	nnode_tmp->set_left(fleft);
	nnode_tmp->set_right(fright);

	//Create ouput node with the right size, that should be U1.0
	std::shared_ptr<frepr_arith> nnode(align_stream(nnode_tmp,dt));

	//Create flipflop
	std::string bn("buffer");
	var_property::var_kind vk(var_property::REGULAR);
	std::shared_ptr<frepr_ffd> nnode_ffd(std::make_shared<frepr_ffd>(node_cnt++,dt,bn,vk));
	nnode_ffd->set_src(nnode);
	nnode_ffd->set_clk(mclock);
	nnode_ffd->set_en(ret_p.first);

	//Add to curr_pulse
	curr_pulse.put(node,ret_p.second);
	
	//Add to gen_frepr
	std::shared_ptr<frepr_node> gen_nnode_ffd(nnode_ffd);
	gen_frepr.put(node,gen_nnode_ffd);
}

void frepr_gen::solve_mod(std::shared_ptr<arch_mod> node){
	//std::cerr<<"mod"<<" "<<node->get_node_id()<<std::endl;

	//Create fix bin operation
	std::shared_ptr<arch_node> right(node->get_right());
	std::shared_ptr<arch_node> left(node->get_left());
	unsigned int pid(node->get_pulse_id());
	std::pair<std::shared_ptr<frepr_ffd>,std::pair<unsigned int,unsigned int>> ret_p;
	std::pair<std::shared_ptr<frepr_arith>,std::shared_ptr<frepr_arith>> fixed_v(resolve_bin(right,left,pid,ret_p));	

	//Create node
	//Dont align left and right to output, but to a temporary bigger datatype that can represent both datatypes
	datatype dt(node->get_datatype());
	datatype dt_tmp(inclusive_datatype(dt.get_datatype(),right->get_datatype(),left->get_datatype()));

	std::shared_ptr<frepr_mod> nnode_tmp(std::make_shared<frepr_mod>(node->get_node_id(),dt_tmp));
	std::shared_ptr<frepr_arith> fleft(align_stream(fixed_v.first,dt_tmp));
	std::shared_ptr<frepr_arith> fright(align_stream(fixed_v.second,dt_tmp));
	nnode_tmp->set_left(fleft);
	nnode_tmp->set_right(fright);

	//Create ouput node with the right size
	std::shared_ptr<frepr_arith> nnode(align_stream(nnode_tmp,dt));

	//Create flipflop
	std::string bn("buffer");
	var_property::var_kind vk(var_property::REGULAR);
	std::shared_ptr<frepr_ffd> nnode_ffd(std::make_shared<frepr_ffd>(node_cnt++,dt,bn,vk));
	nnode_ffd->set_src(nnode);
	nnode_ffd->set_clk(mclock);
	nnode_ffd->set_en(ret_p.first);

	//Add to curr_pulse
	curr_pulse.put(node,ret_p.second);
	
	//Add to gen_frepr
	std::shared_ptr<frepr_node> gen_nnode_ffd(nnode_ffd);
	gen_frepr.put(node,gen_nnode_ffd);
}

void frepr_gen::solve_cast(std::shared_ptr<arch_cast> node){
	//std::cerr<<"cast"<<" "<<node->get_node_id()<<std::endl;
	
	//No delay operation
	
	//Get curr_pulse associated to target branch and bind to this one
	std::pair<unsigned int,unsigned int> cp(*(curr_pulse.get(node->get_target())));
	curr_pulse.put(node,cp);
	
	//Create node
	datatype dt(node->get_datatype());
	//std::shared_ptr<frepr_resize> nnode(std::make_shared<frepr_resize>(node->get_node_id(),dt));
	std::shared_ptr<frepr_arith> target(std::static_pointer_cast<frepr_arith>(*gen_frepr.get(node->get_target())));
	std::shared_ptr<frepr_arith> ftarget(align_stream(target,dt));
	//nnode->set_target(ftarget);

	//Add to gen_frepr
	std::shared_ptr<frepr_node> gen_nnode(ftarget); //nnode
	gen_frepr.put(node,gen_nnode);
}

//Can be terminal node
void frepr_gen::solve_id(std::shared_ptr<arch_id> node){
	//std::cerr<<"id"<<" "<<node->get_node_id()<<std::endl;
	
	std::vector<std::shared_ptr<arch_node>> &ins(graph.get_inv_adj_list()[node]);
	if(ins.size()>1){
		std::cerr<<"frepr_gen::solve_id: too many ins"<<std::endl;
		assert(false);
	}

	//In certain conditions, a ffd can be saved
	if(node->get_vk()==var_property::OUT){
		//Find if the in is a ffd and if the pulse is the same
		std::shared_ptr<frepr_node> last(*(gen_frepr.get(ins[0])));
		if(last->get_op()==frepr_node::FFD&&ins[0]->get_pulse_id()==node->get_pulse_id()){
			std::string vn(node->get_name());
			std::string cbkn(node->get_cblock_name());

			//Reuse last ffd as out, to avoid uncessary delays
			std::shared_ptr<frepr_ffd> lffd(std::static_pointer_cast<frepr_ffd>(last));
			lffd->set_vk(var_property::OUT);
			//lffd->set_name(lffd->get_name()+"/"+vn);
			lffd->set_name(vn);
			lffd->set_cblock_name(cbkn);

			curr_pulse.put(node,*(curr_pulse.get(ins[0])));
			gen_frepr.put(node,last);
			return; 
		}
	}

	//Create curr_pulse associated to this branch
	std::shared_ptr<frepr_ffd> pulse_ffd;
	std::pair<unsigned int,unsigned int> start_pair;
	if(ins.size()==1){ //Can continue chain
		start_pair=*(curr_pulse.get(ins[0]));
		pulse_ffd=get_pulse(&start_pair,node->get_pulse_id());
	}
	else{ //Has to start new chain
		start_pair=std::make_pair(node->get_pulse_id(),0); 
		pulse_ffd=start_chain(node->get_pulse_id());
	}
	curr_pulse.put(node,start_pair);
	
	//Create node
	std::string vn(node->get_name());
	std::string cbkn(node->get_cblock_name());
	datatype dt(node->get_datatype());
	var_property::var_kind vk(node->get_vk());
	std::shared_ptr<frepr_ffd> nnode(std::make_shared<frepr_ffd>(node->get_node_id(),dt,vn,vk));
	nnode->set_en(pulse_ffd);
	nnode->set_clk(mclock);	
	nnode->set_cblock_name(cbkn);
	
	//Get src, if existent
	if(ins.size()==1){
		nnode->set_src(*gen_frepr.get(ins[0]));
	}
	
	//Add to gen_frepr
	std::shared_ptr<frepr_node> gen_nnode(nnode);
	gen_frepr.put(node,gen_nnode);
}

//Can be terminal node
void frepr_gen::solve_ins(std::shared_ptr<arch_ins> node){
	//std::cerr<<"ins"<<" "<<node->get_node_id()<<std::endl;
	
	//Create curr_pulse associated to this branch
	std::pair<unsigned int,unsigned int> start_pair(std::make_pair(node->get_pulse_id(),0));
	std::shared_ptr<frepr_ffd> pulse_ffd(start_chain(node->get_pulse_id()));
	curr_pulse.put(node,start_pair);
	
	//Create flipflop
	std::string bn("ins");
	datatype dt(node->get_datatype());

	var_property::var_kind vk(var_property::REGULAR);
	std::shared_ptr<frepr_ffd> nnode_ffd(std::make_shared<frepr_ffd>(node_cnt++,dt,bn,vk));

	//Dont assume target exists
	std::shared_ptr<frepr_arith> target(nullptr); 
	ins_map.insert(std::make_pair(nnode_ffd,node->get_target()));

	nnode_ffd->set_src(target);
	nnode_ffd->set_clk(mclock);
	nnode_ffd->set_en(pulse_ffd);

	//Default values must be treated on ins_map since the default value must be pushed back
	//Example:
	//default value of -1 goes to 0
	//-2 to -1 etc
	//Although they are set here, on the execute function, they are shifted back!
	if(!node->get_default_value().empty()){
		nnode_ffd->set_default_val(node->get_default_value());
	}
	
	//Add to gen_frepr
	std::shared_ptr<frepr_node> gen_nnode_ffd(nnode_ffd);
	gen_frepr.put(node,gen_nnode_ffd);

}

//Terminal node
void frepr_gen::solve_num(std::shared_ptr<arch_num> node){
	//std::cerr<<"num"<<" "<<node->get_node_id()<<std::endl;

	//Create curr_pulse associated to this branch
	std::pair<unsigned int,unsigned int> start_pair(std::make_pair(0,0)); //PID 0 = CONST
	curr_pulse.put(node,start_pair);

	std::shared_ptr<frepr_ffd> ffd(start_chain(node->get_pulse_id()));

	//Create node
	datatype dt(node->get_datatype());
	std::shared_ptr<frepr_const> nnode(std::make_shared<frepr_const>(node->get_node_id(),dt,node->get_num()));

	//Add to gen_frepr
	std::shared_ptr<frepr_node> gen_nnode(nnode);
	gen_frepr.put(node,gen_nnode);
}

