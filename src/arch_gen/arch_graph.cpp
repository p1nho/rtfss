#include "arch_graph.h"

void arch_graph::add_node(std::shared_ptr<arch_node> node){
	nodes[node->get_node_id()]=node;
	inv_adj_list[node]; //This instantiates std::vector<std::shared_ptr<arch_node>>
	adj_list[node]; //This instantiates std::vector<std::shared_ptr<arch_node>>
}

void arch_graph::rm_node(std::shared_ptr<arch_node> node){
	//Assume there are no relations of this node
	nodes.erase(node->get_node_id());
	inv_adj_list.erase(node);
	adj_list.erase(node);
}

std::shared_ptr<arch_node> arch_graph::get_node(unsigned int node){
	return nodes[node];
}

void arch_graph::add_dep(std::shared_ptr<arch_node> base,std::shared_ptr<arch_node> dep){
	add_node(dep);
	add_node(base);

	adj_list[dep].push_back(base);
	inv_adj_list[base].push_back(dep);
}

void arch_graph::rm_dep(std::shared_ptr<arch_node> base,std::shared_ptr<arch_node> dep){
	auto al_pos(std::find(adj_list[dep].begin(),adj_list[dep].end(),base));
	auto ial_pos(std::find(inv_adj_list[base].begin(),inv_adj_list[base].end(),dep));

	if(al_pos!=adj_list[dep].end()){
		adj_list[dep].erase(al_pos);
	}
	else{
		assert(false);
	}

	if(ial_pos!=inv_adj_list[base].end()){
		inv_adj_list[base].erase(ial_pos);
	}
	else{
		assert(false);
	}
}

std::map<std::shared_ptr<arch_node>,std::vector<std::shared_ptr<arch_node>>,std::owner_less<std::shared_ptr<arch_node>>>& arch_graph::get_inv_adj_list(){
	return inv_adj_list;
}

std::map<std::shared_ptr<arch_node>,std::vector<std::shared_ptr<arch_node>>,std::owner_less<std::shared_ptr<arch_node>>>& arch_graph::get_adj_list(){
	return adj_list;
}

std::map<unsigned int,std::shared_ptr<arch_node>>& arch_graph::get_nodes(){
	return nodes;
}

void arch_graph::gen_gv(std::stringstream &ss,bool top_level){
	std::string gv_label(arch_node::gv_label);

    if(top_level){
		//Begin graph
		ss<<"digraph {"<<std::endl;
	}

	//In order for the nodes to be shown with the correct pids, inverse adj_list traversal needed
	//For each pulse
	for(unsigned int i=0;i<pulse_cnt;i++){
		//Iterate adjacency list

		ss<<"subgraph cluster_"<<i<<" {"<<std::endl;
		ss<<"label=\"pid "<<i<<"\";"<<std::endl;

		//Resolve nodes with inverse adjancency list
		for(auto it=inv_adj_list.begin();it!=inv_adj_list.end();it++){
			std::shared_ptr<arch_node> to(it->first);

			if(to->get_pulse_id()!=i) continue; //Jump

			//Create node
			ss<<gv_label<<to->get_node_id()<<" [ "
				<<"label=\""<<to->to_string()<<"\""
				//<<"; style=bold"
				<<" ]"<<std::endl;
		}

		ss<<"}"<<std::endl;
	}

	//Resolve adjacencies with normal adjancency list
	for(auto it=adj_list.begin();it!=adj_list.end();it++){
		std::shared_ptr<arch_node> from(it->first);
		std::vector<std::shared_ptr<arch_node>> to(it->second);

		for(auto it0=to.begin();it0!=to.end();it0++){
			//If node is arch_ins, then paint connection a different color
			if((*it0)->get_op()==arch_node::INS){
				//Add connections
				ss<<gv_label<<from->get_node_id()<<" -> "
					<<gv_label<<(*it0)->get_node_id()<<" [ "
					<<"color="<<"blue"
					<<" ]"<<std::endl;
			}
			else{
				//Add connections
				ss<<gv_label<<from->get_node_id()<<" -> "
					<<gv_label<<(*it0)->get_node_id()<<" [ "
					//<<"label="<<"datatype"
					<<" ]"<<std::endl;
			}
		}

		//Add specific extra connections
		from->gz_extra_conn(ss);
	}
	
	if(top_level){
		//End graph
		ss<<"}"<<std::endl;
	}
}
