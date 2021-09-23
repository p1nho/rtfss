#ifndef RTFSS_DEF_FREPR_NODE
#define RTFSS_DEF_FREPR_NODE

#include <boost/dynamic_bitset/dynamic_bitset.hpp>
#include <vector>
#include <map>
#include <string>
#include <memory>
#include <algorithm>
#include <sstream>
#include <boost/dynamic_bitset.hpp>

#include "../ast/ast.h"
#include "../var_solver/var.h"

//TODO create a namespace?

class frepr_node{
	//Final representation, with hardware operation
	//Base class to all the possible nodes

	public:
		enum frepr_op{
			ANCHOR,			//Final anchor
			CLOCK,			//System clock
			FFD,			//Flip-Flop D
			PULSER,			//Pulser
			ADD,			//Addition
			SUB,			//Subtraction
			SIM,			//Simetric
			MULT,			//Multiplication
			DIV,			//Division
			SHL,			//Shift left
			SHR,			//Shift right
			RTL,			//Rotate left
			RTR,			//Rotate right
			AND,			//AND gate
			OR,				//OR gate
			XOR,			//XOR gate
			NOT,			//NOT gate
			COMP,			//Logic Comparison
			MOD,			//Modulo operation
			RESIZE,			//Resize operation
			CONST,			//Constant
							//TODO BITWISE LOGIC OPS 
		};

	protected:
		frepr_node(frepr_op oper,unsigned int frepr_id):
			op(oper),
			frepr_id(frepr_id){} //Force "interface" class
	private:
		frepr_op op;

		unsigned int frepr_id;

	public:
		//virtual std::shared_ptr<frepr_node> duplicate()=0;

		frepr_op get_op();
		unsigned int get_node_id();

		virtual std::vector<std::shared_ptr<frepr_node>> get_ins()=0;

		static std::string gv_label;
		//virtual void gz_build(std::stringstream &ss)=0;
		//virtual std::string to_string()=0;

};

class frepr_anchor : public frepr_node{
	private:
		std::vector<std::shared_ptr<frepr_node>> end_nodes;
	public:
		frepr_anchor():
			frepr_node(ANCHOR,-1){}

		void add_end_node(std::shared_ptr<frepr_node> node);
		std::vector<std::shared_ptr<frepr_node>>& get_nodes();
		virtual std::vector<std::shared_ptr<frepr_node>> get_ins();
};

class frepr_clock : public frepr_node{
	//TODO store clock speed
	public:
		frepr_clock():
			frepr_node(CLOCK,-1){}
		virtual std::vector<std::shared_ptr<frepr_node>> get_ins();
};

class frepr_arith : public frepr_node{ //frepr arithmetic operations
	private:
		datatype dt;

	protected:
		frepr_arith(frepr_op oper,unsigned int node_id,datatype& dt):
			frepr_node(oper,node_id),
			dt(dt){} //Force "interface" class
	public:
		datatype get_datatype();

		virtual std::vector<std::shared_ptr<frepr_node>> get_ins()=0;

		//virtual std::string to_string()=0;
};

class frepr_ffd : public frepr_arith{ 
	private:
		std::string name;
		std::string cblock_name;
		boost::dynamic_bitset<> default_val;

		std::shared_ptr<frepr_node> src; //Source
		std::shared_ptr<frepr_node> clk; //Clock
		std::shared_ptr<frepr_node> en;  //Enable

		var_property::var_kind vk;

	public:
		frepr_ffd(unsigned int node_id,datatype& dt,std::string& name,var_property::var_kind &vk):
			frepr_arith(FFD,node_id,dt),
			name(name),
			vk(vk){}

		void set_src(std::shared_ptr<frepr_node> src);
		void set_clk(std::shared_ptr<frepr_node> clk);
		void set_en(std::shared_ptr<frepr_node> en);
		void set_vk(var_property::var_kind vk);
		void set_default_val(boost::dynamic_bitset<> dv);
		void set_name(std::string name);
		void set_cblock_name(std::string cblk_name);

		std::shared_ptr<frepr_node> get_src();
		std::shared_ptr<frepr_node> get_clk();
		std::shared_ptr<frepr_node> get_en();
		boost::dynamic_bitset<> get_default_val();

		std::string get_cblock_name();
		std::string get_name();
		var_property::var_kind get_vk();

		virtual std::vector<std::shared_ptr<frepr_node>> get_ins();
		//virtual std::shared_ptr<frepr_node> duplicate();

		//virtual std::string to_string();
};

class frepr_pulser : public frepr_node{ 
	private:
		time_scale ts;
		double val;
	public:
		frepr_pulser(unsigned int node_id):
			frepr_node(PULSER,node_id){}

		void set_ts(time_scale ts);
		void set_val(double val);

		time_scale get_ts();
		double get_val();

		virtual std::vector<std::shared_ptr<frepr_node>> get_ins();
		//virtual std::shared_ptr<frepr_node> duplicate();

		//virtual std::string to_string();
};

class frepr_unary_arith : public frepr_arith{ //Arch unary operators arithmetic
	//Class can't be instantiated alone
	private:
		std::shared_ptr<frepr_arith> target;
	protected:
		//std::shared_ptr<frepr_node> duplicate_unary(std::shared_ptr<frepr_unary_arith> &dup_self);

		frepr_unary_arith(frepr_op oper,unsigned int node_id,datatype& dt):
			frepr_arith(oper,node_id,dt){} 
	public:
		void set_target(std::shared_ptr<frepr_arith> &target);
		std::shared_ptr<frepr_arith> get_target();

		virtual std::vector<std::shared_ptr<frepr_node>> get_ins();
		//virtual std::shared_ptr<frepr_node> duplicate()=0;
		
		//virtual std::string to_string()=0;
};

class frepr_bin_arith : public frepr_arith{ //Arch binary operators arithmetic
	//Class can't be instantiated alone
	private:
		std::shared_ptr<frepr_arith> left;
		std::shared_ptr<frepr_arith> right;
	protected:
		//std::shared_ptr<frepr_node> duplicate_binary(std::shared_ptr<frepr_bin_arith> &dup_self);

		frepr_bin_arith(frepr_op oper,unsigned int node_id,datatype& dt):
			frepr_arith(oper,node_id,dt){} 
	public:
		void set_left(std::shared_ptr<frepr_arith> &left);
		void set_right(std::shared_ptr<frepr_arith> &right);
		std::shared_ptr<frepr_arith> get_left();
		std::shared_ptr<frepr_arith> get_right();

		virtual std::vector<std::shared_ptr<frepr_node>> get_ins();
		//virtual std::shared_ptr<frepr_node> duplicate()=0;
		
		//virtual std::string to_string()=0;
};

class frepr_add : public frepr_bin_arith{
	private:
	public:
		frepr_add(unsigned int node_id,datatype& dt):
			frepr_bin_arith(ADD,node_id,dt){}

		//virtual std::shared_ptr<frepr_node> duplicate();
		
		//virtual std::string to_string();
};

class frepr_sub : public frepr_bin_arith{
	private:
	public:
		frepr_sub(unsigned int node_id,datatype& dt):
			frepr_bin_arith(SUB,node_id,dt){}

		//virtual std::shared_ptr<frepr_node> duplicate();
		
		//virtual std::string to_string();

};

class frepr_sim : public frepr_unary_arith{
	private:
	public:
		frepr_sim(unsigned int node_id,datatype& dt):
			frepr_unary_arith(SIM,node_id,dt){}

		//virtual std::shared_ptr<frepr_node> duplicate();
		
		//virtual std::string to_string();

};

class frepr_mult : public frepr_bin_arith{
	private:
	public:
		frepr_mult(unsigned int node_id,datatype& dt):
			frepr_bin_arith(MULT,node_id,dt){}

		//virtual std::shared_ptr<frepr_node> duplicate();
		
		//virtual std::string to_string();

};

class frepr_div : public frepr_bin_arith{
	private:
	public:
		frepr_div(unsigned int node_id,datatype& dt):
			frepr_bin_arith(DIV,node_id,dt){}

		//virtual std::shared_ptr<frepr_node> duplicate();
		
		//virtual std::string to_string();

};

class frepr_shl : public frepr_bin_arith{
	private:
	public:
		frepr_shl(unsigned int node_id,datatype& dt):
			frepr_bin_arith(SHL,node_id,dt){}

		//virtual std::shared_ptr<frepr_node> duplicate();
		
		//virtual std::string to_string();

};

class frepr_shr : public frepr_bin_arith{
	private:
	public:
		frepr_shr(unsigned int node_id,datatype& dt):
			frepr_bin_arith(SHR,node_id,dt){}

		//virtual std::shared_ptr<frepr_node> duplicate();
		
		//virtual std::string to_string();

};

class frepr_rtl : public frepr_bin_arith{
	private:
	public:
		frepr_rtl(unsigned int node_id,datatype& dt):
			frepr_bin_arith(RTL,node_id,dt){}

		//virtual std::shared_ptr<frepr_node> duplicate();
		
		//virtual std::string to_string();

};

class frepr_rtr : public frepr_bin_arith{
	private:
	public:
		frepr_rtr(unsigned int node_id,datatype& dt):
			frepr_bin_arith(RTR,node_id,dt){}

		//virtual std::shared_ptr<frepr_node> duplicate();
		
		//virtual std::string to_string();

};

class frepr_and : public frepr_bin_arith{
	private:
	public:
		frepr_and(unsigned int node_id,datatype& dt):
			frepr_bin_arith(AND,node_id,dt){}

		//virtual std::shared_ptr<frepr_node> duplicate();
		
		//virtual std::string to_string();

};

class frepr_or : public frepr_bin_arith{
	private:
	public:
		frepr_or(unsigned int node_id,datatype& dt):
			frepr_bin_arith(OR,node_id,dt){}

		//virtual std::shared_ptr<frepr_node> duplicate();
		
		//virtual std::string to_string();

};

class frepr_xor : public frepr_bin_arith{
	private:
	public:
		frepr_xor(unsigned int node_id,datatype& dt):
			frepr_bin_arith(XOR,node_id,dt){}

		//virtual std::shared_ptr<frepr_node> duplicate();
		
		//virtual std::string to_string();

};

class frepr_not : public frepr_unary_arith{
	private:
	public:
		frepr_not(unsigned int node_id,datatype& dt):
			frepr_unary_arith(NOT,node_id,dt){}

		//virtual std::shared_ptr<frepr_node> duplicate();
		
		//virtual std::string to_string();

};

class frepr_comp : public frepr_bin_arith{
	private:
		logic_op op;
	public:
		frepr_comp(unsigned int node_id,logic_op op,datatype& dt):
			frepr_bin_arith(COMP,node_id,dt),
			op(op){}

		logic_op get_logic_op();

		//virtual std::shared_ptr<frepr_node> duplicate();
		
		//virtual std::string to_string();
};

class frepr_mod : public frepr_bin_arith{
	private:
	public:
		frepr_mod(unsigned int node_id,datatype& dt):
			frepr_bin_arith(MOD,node_id,dt){}

		//virtual std::shared_ptr<frepr_node> duplicate();
		
		//virtual std::string to_string();

};

class frepr_resize : public frepr_unary_arith{
	private:
	public:
		frepr_resize(unsigned int node_id,datatype& dt):
			frepr_unary_arith(RESIZE,node_id,dt){}

		//virtual std::shared_ptr<frepr_node> duplicate();
		
		//virtual std::string to_string();

};

class frepr_const : public frepr_arith{ 
	private:
		//std::shared_ptr<ast_num> num;
		boost::dynamic_bitset<> fixed_num;

	public:
		frepr_const(unsigned int node_id,datatype& dt,boost::dynamic_bitset<> fixed_num):
			frepr_arith(CONST,node_id,dt),
			fixed_num(fixed_num){}

		boost::dynamic_bitset<> get_const();

		virtual std::vector<std::shared_ptr<frepr_node>> get_ins();
		
		//virtual std::string to_string();
};

#endif
