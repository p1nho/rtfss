#ifndef RTFSS_DEF_ARCH_GRAPH
#define RTFSS_DEF_ARCH_GRAPH

#include <vector>
#include <map>
#include <string>
#include <memory>
#include <algorithm>
#include <sstream>
#include <set>

#include "../ast/ast.h"
#include "arch_node.h"

//TODO create a namespace?

class arch_graph{
	//Graph support class
	//Used to build the graph architecture

	private:
		std::map<std::shared_ptr<arch_node>,std::vector<std::shared_ptr<arch_node>>,std::owner_less<std::shared_ptr<arch_node>>> adj_list;
		std::map<std::shared_ptr<arch_node>,std::vector<std::shared_ptr<arch_node>>,std::owner_less<std::shared_ptr<arch_node>>> inv_adj_list;
		std::map<unsigned int,std::shared_ptr<arch_node>> nodes; //TODO should be vector?
		unsigned int pulse_cnt;

	public:
		arch_graph(unsigned int pulse_cnt):
			inv_adj_list(),
			pulse_cnt(pulse_cnt){}

		void add_node(std::shared_ptr<arch_node> node);
		void rm_node(std::shared_ptr<arch_node> node);
		std::shared_ptr<arch_node> get_node(unsigned int node);
		void add_dep(std::shared_ptr<arch_node>	base,std::shared_ptr<arch_node> dep);
		void rm_dep(std::shared_ptr<arch_node>	base,std::shared_ptr<arch_node> dep);

		std::map<std::shared_ptr<arch_node>,std::vector<std::shared_ptr<arch_node>>,std::owner_less<std::shared_ptr<arch_node>>>& get_inv_adj_list();
		std::map<std::shared_ptr<arch_node>,std::vector<std::shared_ptr<arch_node>>,std::owner_less<std::shared_ptr<arch_node>>>& get_adj_list();
		std::map<unsigned int,std::shared_ptr<arch_node>>& get_nodes();

		void gen_gv(std::stringstream &ss,bool top_level=true);
};

#endif
